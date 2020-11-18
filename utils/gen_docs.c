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
<h1>Cool stuff</h1>
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
    char *file;
    char *name;
} documentation_file;

typedef struct t_table_entry {
    char *id;
    array comments;
} table_entry;

// This program assumes it being started from the root directory of this project.

documentation_file generate_docs_from_file(file_content* content);
void write_title_page(char* build_folder);
void write_doc_pages(char* build_folder, array* pages, char* code_folder);
void get_item_id(s32 h, char*s, s32 i, char *id_buf, char* link_buf);

int main(int argc, char **argv) {
    char code_folder[MAX_INPUT_LENGTH];
    char build_folder[MAX_INPUT_LENGTH];
    get_active_directory(code_folder);
    get_active_directory(build_folder);
    string_appendn(code_folder, "\\src\\", MAX_INPUT_LENGTH);
    string_appendn(build_folder, "\\build\\", MAX_INPUT_LENGTH);

    printf("Generating docs for %s\n", code_folder);

    if (!platform_directory_exists(code_folder)) {
        printf("Code folder does not exist. %s\n", code_folder);
        return 1;
    }

    char name[10];
    string_appendn(name, "*.h", 10);
	
	array files = array_create(sizeof(found_file));
	array filters = get_filters(name);
	bool is_cancelled = false;
    bool want_dir = false;
	platform_list_files_block(&files, code_folder, filters, true, 0, want_dir, &is_cancelled, 0);

    array documentation_files = array_create(sizeof(documentation_file));
    array_reserve(&documentation_files, 100);
    s32 len = strlen(code_folder);
    for (s32 i = 0; i < files.length; i++)
	{
		found_file *file = array_at(&files, i);

        printf("Generating docs for file %s\n", file->path);
        if (!platform_file_exists(file->path) || string_contains(file->path, "external")) {
            continue;
        }

        file_content content = platform_read_file_content(file->path, "rb");
        documentation_file result = generate_docs_from_file(&content);
        result.file = file->path +len;
        result.name = mem_alloc(strlen(result.file)+1);
        string_copyn(result.name, result.file, strlen(result.file)-2);
        array_push(&documentation_files, &result);
	}

    write_doc_pages(build_folder, &documentation_files, code_folder);
    write_title_page(build_folder);

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

    string_convert_crlf_to_lf(content->content);
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
            bool is_definition = string_contains_ex(line, "#define * ", &matches, &cancel);
            bool is_function = string_contains_ex(line, "* *(*);", &matches, &cancel);
            bool is_comment = string_contains_ex(line, "//", &matches, &cancel);

            if (is_definition) {
                array_push(&new_f.definitions, &line);
            }
            else if (is_function) {
                function def;
                def.comments = array_copy(&comments);
                comments.length = 0;
                def.content = line;
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

void write_doc_pages(char* build_folder, array* pages, char* code_folder) {
    char str[MAX_INPUT_LENGTH];
    string_copyn(str, build_folder, MAX_INPUT_LENGTH);
    string_appendn(str, "docs.html", MAX_INPUT_LENGTH);

    char changelog[MAX_INPUT_LENGTH];
    string_copyn(changelog, build_folder, MAX_INPUT_LENGTH);
    string_appendn(changelog, "..\\README", MAX_INPUT_LENGTH);

    s32 size = megabytes(10);
    char* tmp = mem_alloc(size);
    tmp[0] = 0;

    string_appendn(tmp, "<html>", size);
    string_appendn(tmp, "<body>", size);

    for (s32 i = 0; i < pages->length; i++) {
        documentation_file *f = array_at(pages, i);

        array comments = array_create(sizeof(table_entry));
        array_reserve(&comments, 200);

        bool is_valid = false;
        for (s32 d = 0; d < f->docs.length; d++) {
            char *doc_str = *(char**)array_at(&f->docs, d);
            if (string_contains(doc_str, "<h1>")) {
                is_valid = true;
            }
        }

        if (!f->definitions.length && !f->structs.length && !f->functions.length) {
            continue;
        }
        
        string_appendn(tmp, "<TITLE>Project-base Technical Reference Manual</TITLE>", size);      

        if (!is_valid) {
            string_appendn(tmp, "<h1>", size);
            string_appendn(tmp, f->name, size);
            string_appendn(tmp, "</h1>", size);
            string_appendn(tmp, "<cite>", size);
            string_appendn(tmp, f->file, size);
            string_appendn(tmp, "</cite>", size);
            string_appendn(tmp, "<hr>", size);
            is_valid = true;
        }

        for (s32 d = 0; d < f->docs.length; d++) {
            char *doc_str = *(char**)array_at(&f->docs, d);
            string_appendn(tmp, doc_str, size);
        }

        // Dont write a page for anthing that doesnt have a description.
        // This description needs to have a <h1> defined.
        if (!is_valid) continue;
        
        // Definitions
        string_appendn(tmp, "<h2>", size);
        string_appendn(tmp, "Definitions", size);
        string_appendn(tmp, "</h2>", size);
        
        if (f->definitions.length) {
            string_appendn(tmp, "<h3>", size);
            string_appendn(tmp, "Constants", size);
            string_appendn(tmp, "</h3>", size);

            string_appendn(tmp, "<pre>", size);
            for (s32 d = 0; d < f->definitions.length; d++) {
                char id_buf[100];
                char link_buf[100];
                get_item_id(i+1, "d", d+1, id_buf, link_buf);
                string_appendn(tmp, link_buf, size);

                string_appendn(tmp, *(char**)array_at(&f->definitions, d), size);
                string_appendn(tmp, "<br>", size);
            }
            string_appendn(tmp, "</pre>", size);
        }

        if (f->structs.length) {
            string_appendn(tmp, "<h3>", size);
            string_appendn(tmp, "Structures", size);
            string_appendn(tmp, "</h3>", size);
            
            string_appendn(tmp, "<pre>", size);
            for (s32 d = 0; d < f->structs.length; d++) {
                struct_def *def = array_at(&f->structs, d);

                char id_buf[100];
                char link_buf[100];
                get_item_id(i+1, "s", d+1, id_buf, link_buf);
                string_appendn(tmp, link_buf, size);
                string_appendn(tmp, "<br>", size);

                string_appendn(tmp, def->content, size);
                string_appendn(tmp, "<br>", size);
                string_appendn(tmp, "<br>", size);

                if (def->comments.length) {
                    table_entry entry;
                    entry.id = mem_alloc(100);
                    string_copyn(entry.id, id_buf, 100);
                    entry.comments = def->comments;
                    array_push(&comments, &entry);
                }
            }
            string_appendn(tmp, "</pre>", size);
        }

        if (f->functions.length) {
            string_appendn(tmp, "<h3>", size);
            string_appendn(tmp, "Methods", size);
            string_appendn(tmp, "</h3>", size);
                
            string_appendn(tmp, "<pre>", size);
            for (s32 d = 0; d < f->functions.length; d++) {
                function *def = array_at(&f->functions, d);

                char id_buf[100];
                char link_buf[100];
                get_item_id(i+1, "f", d+1, id_buf, link_buf);
                string_appendn(tmp, link_buf, size);

                string_appendn(tmp, def->content, size);
                string_appendn(tmp, "<br>", size);

                if (def->comments.length) {
                    table_entry entry;
                    entry.id = mem_alloc(100);
                    string_copyn(entry.id, id_buf, 100);
                    entry.comments = def->comments;
                    array_push(&comments, &entry);
                }
            }
            string_appendn(tmp, "</pre>", size);

            // Comments
            if (comments.length) {
                string_appendn(tmp, "<h2>", size);
                string_appendn(tmp, "Explaination", size);
                string_appendn(tmp, "</h2>", size);
                

                string_appendn(tmp, "<table>", size);

                string_appendn(tmp, "<tr>", size);

                string_appendn(tmp, "<th>", size);
                string_appendn(tmp, "#", size);
                string_appendn(tmp, "<hr>", size);
                string_appendn(tmp, "</th>", size);

                string_appendn(tmp, "<th>", size);
                string_appendn(tmp, "Comments", size);
                string_appendn(tmp, "<hr>", size);
                string_appendn(tmp, "</th>", size);

                string_appendn(tmp, "</tr>", size);

                for (s32 x = 0; x < comments.length; x++) {
                    table_entry* entry = array_at(&comments, x);
                    
                    string_appendn(tmp, "<tr>", size);

                    string_appendn(tmp, "<td>", size);

                    string_appendn(tmp, "<a name=\"", size);
                    string_appendn(tmp, entry->id, size);
                    string_appendn(tmp, "\">", size);

                    string_appendn(tmp, "<cite>", size);
                    string_appendn(tmp, entry->id, size);
                    string_appendn(tmp, "</cite>", size);
                    string_appendn(tmp, "</td>", size);

                    string_appendn(tmp, "<td>", size);
                    string_appendn(tmp, "<pre>", size);
                    for (s32 x = 0; x < entry->comments.length; x++) {
                        string_appendn(tmp, *(char**)array_at(&entry->comments, x), size);
                        string_appendn(tmp, "<br>", size);
                    }
                    string_appendn(tmp, "</pre>", size);
                    string_appendn(tmp, "</td>", size);

                    string_appendn(tmp, "</tr>", size);
                }

                string_appendn(tmp, "</table>", size);
            }
        }
    }

    file_content content = platform_read_file_content(changelog, "rb");
    if (content.content) {
        string_appendn(tmp, "<h1>", size);
        string_appendn(tmp, "History", size);
        string_appendn(tmp, "</h1>", size);

        string_appendn(tmp, "<pre>", size);
        string_appendn(tmp, content.content, size);
        string_appendn(tmp, "</pre>", size);
    }

    string_appendn(tmp, "</body>", size);
    string_appendn(tmp, "</html>", size);

    platform_write_file_content(str, "w+", tmp, strlen(tmp));
}

void get_item_id(s32 h, char*s, s32 i, char *id_buf, char* link_buf) {
    s32 max_length = 7;
    sprintf(id_buf, "%d%s%d", h, s, i);
    s32 len = strlen(id_buf);
    sprintf(link_buf, "<a href=\"#%s\">", id_buf);
    string_appendn(link_buf, id_buf, 100);
    string_appendn(link_buf, "</a>", 100);
    for (s32 i = 0; i < max_length - len; i++) {
       string_appendn(link_buf, "&nbsp;", 100);
    }
}

void write_title_page(char* build_folder) {
    char tmp[MAX_INPUT_LENGTH];
    string_copyn(tmp, build_folder, MAX_INPUT_LENGTH);
    string_appendn(tmp, "docs_title.html", MAX_INPUT_LENGTH);

    char* str = 
    "<h1>Project-base Technical Reference Manual</h1><hr>"
    "<p>Written by Aldrik Ramaekers<br>This document is distributed under the BSD 2-Clause 'Simplified' License.</p>"
    "<cite>This document pertains to version 2.0.0 of the project-base library.</cite>"
    "<br><br>"
    "<h1>Introduction</h1>"
    "<p>This document gives a technical description for the Project-base library."
    "The Project-base library is a general purpose library intended for creating graphical programs for the Windows and Linux operating system."
    "This document describes all the components of the Project-base library and gives examples for using these components.</p>"
    "<br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br>"
    "<BLOCKQUOTE>© Aldrik Ramaekers, 2020<br>https://aldrik.org<br>aldrik.ramaekers@protonmail.com</BLOCKQUOTE>";

    platform_write_file_content(tmp, "w+", str, strlen(str));
}