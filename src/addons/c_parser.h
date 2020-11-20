#ifndef INCLUDE_ADDON_C_PARSER
#define INCLUDE_ADDON_C_PARSER

typedef enum t_c_token_type {
    C_TOKEN_BLOCK_OPEN, // {
    C_TOKEN_BLOCK_CLOSE, // }
    C_TOKEN_PAREN_OPEN, // (
    C_TOKEN_PAREN_CLOSE, // )
    C_TOKEN_COMMA,
    C_TOKEN_COMMENT_START, // /*
    C_TOKEN_COMMENT_END, // */
    C_TOKEN_BRACKET_OPEN, // [
    C_TOKEN_BRACKET_CLOSE, // ]    
    C_TOKEN_TYPEDEF,
    C_TOKEN_IDENTIFIER,
    C_TOKEN_STATIC_CONDITION, // #if, #else, #elseif, #endif, #ifdef, #elseif
    C_TOKEN_DEFINE,
    C_TOKEN_UNDEFINE,
    C_TOKEN_INCLUDE,
    C_TOKEN_END_BLOCK, // ;
    C_TOKEN_CONDITION, // if, else, while, for
    C_TOKEN_DO,
    C_TOKEN_ASSIGN, // =, /=, *=, ^=, %=, +=, -=, <<=, >>=, &=, |=
    C_TOKEN_COMPARATOR, // ==, <=, >=, <, >, !=
    C_TOKEN_MODIFIER, // +, -, /, %, ++, --
    C_TOKEN_LOGICAL, // &&, ||, !
    C_TOKEN_BITWISE, // &, |, ^, ~, <<, >>
    C_TOKEN_PTR_OR_MULTIPLY, // *
    C_TOKEN_NEWLINE,
    C_TOKEN_END,
} c_token_type;

typedef struct t_c_token {
    c_token_type    type;
    char*           token;
} c_token;

c_token*    c_parser_tokenize(char* data, int data_length);
void        c_parser_print(c_token* tokens);

#endif