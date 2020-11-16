#include <projectbase/project_base.h>

// Things we want to extract
// struct definitions + comments above this defintion
// struct properties + comments behind this defintion
// function definitions + comments above this defintion
// block comments starting with :docs:

// Things we dont support
// multi line #defines
// multi line definitions

#if 0
example:

/*
:docs:
In order to do things we need to store things.
*/

// Used for storing things
typedef struct t_test {
    s8 data; // Data can be between 0 and 100
} test;

// Retrieve all things
void get_things(s8 source);

#endif

typedef struct t_function {
    char *content;
    array comments;
} function;

typedef struct t_struct_def {
    char *content;
    array comments;
} struct_def;

typedef struct t_documentation_file {
    array docs;
    array structs;
    array definitions;
    array functions;
} documentation_file;

// This program assumes it being started from the root directory of this project.

documentation_file generate_docs_from_file(file_content* content);

int main(int argc, char **argv) {
    char code_folder[MAX_INPUT_LENGTH];
    get_active_directory(code_folder);
    string_appendn(code_folder, "\\src\\", MAX_INPUT_LENGTH);

    printf("Generating docs for %s\n", code_folder);

    if (!platform_directory_exists(code_folder)) {
        printf("Code folder does not exist. %s\n", code_folder);
        return 1;
    }

    char name[10];
    string_appendn(name, "input.h", 10);
	
	array files = array_create(sizeof(found_file));
	array filters = get_filters(name);
	bool is_cancelled = false;
    bool want_dir = false;
	platform_list_files_block(&files, code_folder, filters, true, 0, want_dir, &is_cancelled, 0);

    array documentation_files = array_create(sizeof(documentation_file));
    array_reserve(&documentation_files, 100);

    for (s32 i = 0; i < files.length; i++)
	{
		found_file *file = array_at(&files, i);

        printf("Generating docs for file %s\n", file->path);
        if (!platform_file_exists(file->path)) {
            printf("File doesnt exist.");
            continue;
        }

        file_content content = platform_read_file_content(file->path, "rb");
        documentation_file result = generate_docs_from_file(&content);
        array_push(&documentation_files, &content);

        for (s32 d = 0; d < result.docs.length; d++) {
            printf("DOC FOUND: %s\n", *(char**)array_at(&result.docs, d));
        }
	}

    array_destroy(&filters);
    array_destroy(&files);

    printf("Done generating docs\n");

    return 0;
}

bool is_newline = true;
char* get_next_token(char* text, char* buf) {
    if (!text) return (char*)0;
    buf[0] = 0;
    char* start = text;
    s32 search_length = 0;

    while(*start) {
        char current_char = *start;
        is_newline = false;

        if (current_char == ' ' || current_char == '\n') {
            if (current_char == '\n') is_newline = true;

            buf[search_length] = 0;
            start++;
            return start;
        }

        buf[search_length] = current_char;
        start++;
        search_length++;
    }
     
    return (char*)0;
}

documentation_file generate_docs_from_file(file_content* content) {
    documentation_file new_f;
    new_f.docs = array_create(sizeof(char*));
    new_f.structs = array_create(sizeof(struct_def));
    new_f.definitions = array_create(sizeof(char*));
    new_f.functions = array_create(sizeof(function));

    convert_crlf_to_lf(content->content);
    char *start = content->content;
    s32 length = content->content_length;

    array comments = array_create(sizeof(char*));
    array_reserve(&comments, 50);

    // Get definitions
    char *def_search_start = start;
    s32 line_len = 0;
    while(*def_search_start) {
        char tmp = *def_search_start;

        if (tmp == '\n') {
            if (line_len == 0) {
                def_search_start++;
                continue;
            }

            char *line = mem_alloc(line_len+1);
            string_copyn(line, def_search_start - line_len, line_len);

            array matches = array_create(sizeof(text_match));
            bool cancel = false;
            bool is_definition = string_contains_ex(line, "#define", &matches, &cancel);
            bool is_function = string_contains_ex(line, ");", &matches, &cancel);
            bool is_comment = string_contains_ex(line, "//", &matches, &cancel);

            if (is_definition) {
                printf("DEFI: %s\n", line);
                array_push(&new_f.definitions, &line);
            }
            else if (is_function) {
                function def;
                def.comments = array_copy(&comments);
                comments.length = 0;
                def.content = line;
                printf("FUNC: %s\n", def.content);
                array_push(&new_f.functions, &def);
            }
            else if (is_comment) {
                array_push(&comments, &line);
            }
            
            def_search_start++;
            line_len = 0;
            continue;
        }

        def_search_start++;
        line_len++;
    }

    char token_buf[10000];
    while((start = get_next_token(start, token_buf)) && start) {

        // Clear comments that arent connected to the upcoming struct/function definition
        if (is_newline) {
            comments.length = 0;
        }

        // Get docs
        if (string_equals(token_buf, "/*")) {
            char* docs_start = 0;
            while((start = get_next_token(start, token_buf)) && start) {     
                if (string_equals(token_buf, ":docs:")) {
                    docs_start = start;
                }
                if (string_equals(token_buf, "*/")) {
                    if (docs_start != 0) {
                        s32 length = start - docs_start - 4;
                        char* buf = mem_alloc(length+1);
                        string_copyn(buf, docs_start, length);
                        array_push(&new_f.docs, &buf);
                    }
                    break;
                }
            }
        }

        // Get comments
        if (string_equals(token_buf, "//")) {
            char* comment_start = start;

            while((start = get_next_token(start, token_buf)) && start) {
                if (is_newline) {
                    s32 length = start - comment_start - 1;
                    char* buf = mem_alloc(length+1);
                    string_copyn(buf, comment_start, length);
                    array_push(&comments, &buf);
                    break;
                }
            }
        }

        // Get structs
        if (string_equals(token_buf, "typedef")) {
            struct_def def;
            def.comments = array_copy(&comments);
            comments.length = 0;

            char* def_start = start;

            start = get_next_token(start, token_buf);
            if (string_equals(token_buf, "struct") || string_equals(token_buf, "enum")) {
                while((start = get_next_token(start, token_buf)) && start) {
                    if (string_equals(token_buf, "}")) {
                        start = get_next_token(start, token_buf);

                        s32 len = start - def_start - 1;
                        char* buf = mem_alloc(len+1);
                        string_copyn(buf, def_start, len);
                        def.content = buf;
                        array_push(&new_f.structs, &def);
                        break;
                    }
                }
            }
        }
    }

    return new_f;
}