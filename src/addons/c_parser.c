#include "c_parser.h"
#include "../memory.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define bool char
#define false 0
#define true 1

// Remove extra space that isn't in comments or literals
// Add space before and after all tokens like (, ), { etc
// Change crlf to lf
static char* _c_parser_cleanup_data(char* data, int data_length, int *buffer_length) {
    #define insert_space() if (!last_ch_is_space) {buffer[write_cursor++] = ' ';last_ch_is_space = true;}
    #define insert_char(__ch) {buffer[write_cursor++] = __ch;last_ch_is_space = false;}

    *buffer_length = data_length*2;
    char* buffer = mem_alloc(*buffer_length);
    memset(buffer, 0, *buffer_length);

    bool last_ch_is_space = false;
    bool in_op = false;
    bool in_single_line_comment = false;
    bool in_comment = false;

    int read_cursor = 0;
    int write_cursor = 0;
    while(data[read_cursor] && read_cursor <= data_length) {
        // char prev_ch = read_cursor-1 >= 0 ? data[read_cursor-1] : 0;
        char current_ch = data[read_cursor];
        char next_ch = read_cursor+1 <= data_length ? data[read_cursor+1] : 0;

        if (*buffer_length - write_cursor < 100) { // Less then 100 char spaces left.. realloc buffer.
            *buffer_length *= 2;
            buffer = mem_realloc(buffer, *buffer_length);
        }

        { // Single line comments
            if (in_single_line_comment && current_ch == '\n') {
                in_comment = false;
                in_single_line_comment = false;
                insert_space();
                insert_char('*');
                insert_char('/');
                insert_space();
                goto done_with_char;
            }

            if (current_ch == '/' && next_ch == '/') {
                in_comment = true;
                in_single_line_comment = true;
                insert_space();
                insert_char('/');
                insert_char('*');
                insert_space();
                read_cursor++;
                goto done_with_char;
            }
        }

        { // Multi line comments
            if (current_ch == '*' && next_ch == '/') {
                in_comment = false;
                insert_space();
                insert_char('*');
                insert_char('/');
                insert_space();
                read_cursor++;
                goto done_with_char;
            }

            if (current_ch == '/' && next_ch == '*') {
                in_comment = true;
                insert_space();
                insert_char('/');
                insert_char('*');
                insert_space();
                read_cursor++;
                goto done_with_char;
            }
        }

        if (in_comment) {
            insert_char(current_ch);
            goto done_with_char;
        }

        if (current_ch == '(' || current_ch == ')' || current_ch == '{' || 
            current_ch == '}' || current_ch == '[' || current_ch == ']' || 
            current_ch == ';' || current_ch == ',' || current_ch == '\n') {
            insert_space();
            insert_char(current_ch);
            insert_space();
            goto done_with_char;
        }

        if (current_ch == '=' || current_ch == '/' || current_ch == '*' || current_ch == '^' || 
            current_ch == '%' || current_ch == '+' || current_ch == '-' || current_ch == '&' || 
            current_ch == '|' || current_ch == '!' || current_ch == '~' || current_ch == '<' || 
            current_ch == '>' ) {
            
            if (!in_op) {
                insert_space();
            }
            in_op = true;
        }
        else {
            if (in_op) {
                insert_space();
                in_op = false;
            }
        }
        
        if (last_ch_is_space) { // skip extra spaces
            if (current_ch == ' ') {
                goto done_with_char;
            }
        }

        if (current_ch == ' ') {
            insert_space();
        } else if (current_ch > 32) {
            insert_char(current_ch);
        }

        done_with_char:
        read_cursor++;
    }

    insert_char(0);

    return buffer;
}

static char* _c_parser_get_token(char* buffer) {
    while(*buffer) {
        char ch = *buffer;
        
        if (ch == ' ') {
            *buffer = 0;
            buffer++;
            return buffer;
        }

        buffer++;
    }

    return 0;
}

static bool _is_ptr_or_multiply(char* buffer) {
    while(*buffer) {
        if (*buffer != '*') return false;
        buffer++;
    }
    return true;
}

c_token* c_parser_tokenize(char* data, int data_length) 
{
    #define string_equals(__s, __c) (strcmp(__s, __c) == 0)
    #define found_type(__t) {new_token.type = __t;goto done_with_token;}

    int buffer_length = 0;
    char* buffer = _c_parser_cleanup_data(data, data_length, &buffer_length);

    int token_count = 0;
    int token_buffer_size = 1000;
    c_token *token_list = mem_alloc(sizeof(c_token)*token_buffer_size);
    memset(token_list, 0, token_buffer_size);
    token_list[0].type = C_TOKEN_END;

    char* current_token = buffer;
    while ((buffer = _c_parser_get_token(current_token))) {
        c_token new_token;
        new_token.token = current_token;

        if (string_equals(current_token, "{")) {
            found_type(C_TOKEN_BLOCK_OPEN);
        }
        else if (string_equals(current_token, "}")) {
            found_type(C_TOKEN_BLOCK_CLOSE);
        }
        else if (string_equals(current_token, "(")) {
            found_type(C_TOKEN_PAREN_OPEN);
        }
        else if (string_equals(current_token, ")")) {
            found_type(C_TOKEN_PAREN_CLOSE);
        }
        else if (string_equals(current_token, ",")) {
            found_type(C_TOKEN_COMMA);
        }
        else if (string_equals(current_token, "/*")) {
            found_type(C_TOKEN_COMMENT_START);
        }
        else if (string_equals(current_token, "*/")) {
            found_type(C_TOKEN_COMMENT_END);
        }
        else if (string_equals(current_token, "[")) {
            found_type(C_TOKEN_BRACKET_OPEN);
        }
        else if (string_equals(current_token, "]")) {
            found_type(C_TOKEN_BRACKET_CLOSE);
        }
        else if (string_equals(current_token, "\n")) {
            found_type(C_TOKEN_NEWLINE);
        }
        else if (string_equals(current_token, "typedef")) {
            found_type(C_TOKEN_TYPEDEF);
        }
        else if (string_equals(current_token, "#if") || string_equals(current_token, "#else") ||
                 string_equals(current_token, "#elseif") || string_equals(current_token, "#endif") ||
                 string_equals(current_token, "#ifdef") || string_equals(current_token, "#elseif")) {
            found_type(C_TOKEN_STATIC_CONDITION);
        }
        else if (string_equals(current_token, "#define")) {
            found_type(C_TOKEN_DEFINE);
        }
        else if (string_equals(current_token, "#undef")) {
            found_type(C_TOKEN_UNDEFINE);
        }
        else if (string_equals(current_token, "#include")) {
            found_type(C_TOKEN_INCLUDE);
        }
        else if (string_equals(current_token, ";")) {
            found_type(C_TOKEN_END_BLOCK);
        }
        else if (string_equals(current_token, "if") || string_equals(current_token, "else") ||
                 string_equals(current_token, "while") || string_equals(current_token, "for")) {
            found_type(C_TOKEN_CONDITION);
        }
        else if (string_equals(current_token, "do")) {
            found_type(C_TOKEN_DO);
        }
        else if (string_equals(current_token, "=") || string_equals(current_token, "/=") || 
                 string_equals(current_token, "*=") || string_equals(current_token, "^=") || 
                 string_equals(current_token, "%=") || string_equals(current_token, "+=") || 
                 string_equals(current_token, "-=") || string_equals(current_token, "<<=") || 
                 string_equals(current_token, ">>=") || string_equals(current_token, "&=") || 
                 string_equals(current_token, "|=")) {
            found_type(C_TOKEN_ASSIGN);
        }
        else if (string_equals(current_token, "==") || string_equals(current_token, "<=") || 
                 string_equals(current_token, ">=") || string_equals(current_token, "<") || 
                 string_equals(current_token, ">") || string_equals(current_token, "!=")) {
            found_type(C_TOKEN_COMPARATOR);
        }
        else if (string_equals(current_token, "+") || string_equals(current_token, "-") || 
                 string_equals(current_token, "/") || string_equals(current_token, "%") || 
                 string_equals(current_token, "++") || string_equals(current_token, "--")) {
            found_type(C_TOKEN_MODIFIER);
        }
        else if (string_equals(current_token, "&&") || string_equals(current_token, "||") || 
                 string_equals(current_token, "!")) {
            found_type(C_TOKEN_LOGICAL);
        }
        else if (string_equals(current_token, "&") || string_equals(current_token, "|") || 
                 string_equals(current_token, "^") || string_equals(current_token, "~") || 
                 string_equals(current_token, "<<") || string_equals(current_token, ">>")) {
            found_type(C_TOKEN_BITWISE);
        }
        else if (_is_ptr_or_multiply(current_token)) {
            found_type(C_TOKEN_PTR_OR_MULTIPLY);
        }
        else {
            found_type(C_TOKEN_IDENTIFIER);
        }

        done_with_token:
        if (token_buffer_size - token_count < 100) {
            token_buffer_size *= 2;
            token_list = mem_realloc(token_list, sizeof(c_token)*token_buffer_size);
        }
        token_list[token_count++] = new_token;
        current_token = buffer;
        if (!buffer) break;
    }

    token_list[token_count].token = 0;
    token_list[token_count].type = C_TOKEN_END;

    return token_list;
}

void c_parser_print(c_token* tokens)
{
    while((*tokens).type != C_TOKEN_END) {
        c_token t = *tokens;
        printf("%s ", t.token);

        tokens++;
    }
}