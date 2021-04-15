#include <projectbase/project_base.h>

/*

# Assumptions
    - function declarations are single line.
    - Text blocks are 3 tabs indented
    - function tags are 1 tab indented and prefixed with :/[tag]

*/

#define IDENTIFIER_CONTINUATION 	"//			"
#define IDENTIFIER_PREFIX 			":/"
#define INFO_IDENTIFIER 			"//	:/Info"
#define RETURN_IDENTIFIER 			"//	:/Ret"

typedef enum t_parse_state
{
	PARSING_SEARCHING,
	PARSING_TITLE,
	PARSING_INFO,
	PARSING_TEXT,
	PARSING_RET,
	PARSING_FUNC,
} parse_state;

typedef enum t_data_tag_type
{
    CHAPTER = 1, // title, text
    FUNCTION = 2, // info, return
} data_tag_type;

typedef struct t_data_tag
{
    data_tag_type type;
    char* title;
    char* info;
    char* text;
    char* ret;
    char* func;
} data_tag;

array tags;

data_tag current_tag;

void reset_tag()
{
    current_tag.type = 0;
    current_tag.title = 0;
    current_tag.info = 0;
    current_tag.text = 0;
    current_tag.ret = 0;
	current_tag.func = 0;
}

void set_tag_type(data_tag_type type)
{
    if (current_tag.type == 0) current_tag.type = type;
}

void store_tag()
{
    array_push(&tags, &current_tag);
    reset_tag();
}

char* get_str_after_tab(char *string)
{
    char *last_str = string;
    char *str = last_str;
    while((str = strchr(str, 0x9)) && str != 0) last_str = ++str;
    return last_str;
}

char* str_append_newline(char* str, char* append)
{
	if (str)
	{
		int new_len = strlen(str) + strlen(append) + 1;
		char *newbuf = mem_alloc(new_len);
		memset(newbuf, 0, new_len);
		strcpy(newbuf, str);
		strcat(newbuf, "\n");
		strcat(newbuf, append);
	}
	else
	{
		return append;
	}
}

bool str_is_func_def(char* str)
{
    return string_contains(str, "* *(") && string_contains(str, ");");
}

#define IS_CONTINUATION(_ident) (string_contains(string, _ident) || string_contains(string, IDENTIFIER_CONTINUATION)) 
parse_state current_state = PARSING_SEARCHING;

void parse_line(char *string)
{
    int str_len = strlen(string);

	if (current_state == PARSING_SEARCHING)
	{
		if (string_contains(string, INFO_IDENTIFIER)) { current_state = PARSING_INFO; set_tag_type(FUNCTION); };
		if (string_contains(string, RETURN_IDENTIFIER)) current_state = PARSING_RET;
	}

	if (current_tag.type == FUNCTION) {
		if (str_is_func_def(string))
		{
			current_tag.func = str_append_newline(current_tag.func, get_str_after_tab(string));
			store_tag();
		}
		if (current_state == PARSING_INFO)
		{
			if (!IS_CONTINUATION(INFO_IDENTIFIER)) { current_state = PARSING_SEARCHING; }
			else current_tag.info = str_append_newline(current_tag.info, get_str_after_tab(string));
		}
		if (current_state == PARSING_RET)
		{
			if (!IS_CONTINUATION(RETURN_IDENTIFIER)) { current_state = PARSING_SEARCHING; }
			else current_tag.ret = str_append_newline(current_tag.ret, get_str_after_tab(string));
		}
	}
}

void parse_file(char* path)
{
    file_content content = platform_read_file_content(path, "rb");
    char* current_line = content.content;
    char* ptr = current_line;

    while(*ptr != 0)
    {
        if (*ptr == '\n'){
            *ptr = 0;
            parse_line(current_line);
            current_line = ++ptr;
            continue;
        }
        ptr++;
    }
}

#define MAX_BODY_LEN 100000
#define APPEND(_big, _str) strncat(_big, _str, MAX_BODY_LEN);

void dump_html()
{
    char* header =
    "<html>"
    "<body>"
    "<h1>Project-base Technical Reference Manual</h1>"
    "<hr>"
    "<p>Written by Aldrik Ramaekers<br>This document is distributed under the BSD 2-Clause 'Simplified' License.</p><cite>This document pertains to version 2.0.0 of the project-base library.</cite><br><br>"
    "<h1>Introduction</h1>"
    "<p>This document gives a technical description for the Project-base library.The Project-base library is a general purpose library intended for creating graphical programs for the Windows and Linux operating system.This document describes all the components"
    "of the Project-base library and gives examples for using these components.</p>"
    "<b>© Aldrik Ramaekers, 2020<br>https://aldrik.org<br>aldrik.ramaekers@protonmail.com</b>"
    "</body>"
    "</html>";

    char* body = mem_alloc(MAX_BODY_LEN);
    memset(body, 0, MAX_BODY_LEN);

    APPEND(body, "<html>");
    APPEND(body, "<body>");

    for (int i = 0; i < tags.length; i++)
    {
        data_tag* tag = array_at(&tags, i);

        if (tag->type == CHAPTER)
        {
			printf("chapter: %s\n", tag->title);
			printf("	text: %s\n", tag->text);
            APPEND(body, "<h1>");
            APPEND(body, tag->title);
            APPEND(body, "</h1>");
        }
		if (tag->type == FUNCTION)
        {
			printf("func\n");
			printf("	info: %s\n", tag->info);
			printf("	ret: %s\n", tag->ret);
			printf("	desc: %s\n", tag->func);
        }
    }

    APPEND(body, "</body>");
    APPEND(body, "</html>");

    platform_write_file_content("build\\docs_title.html", "wb+", header, strlen(header));
	platform_write_file_content("build\\docs.html", "wb+", body, strlen(body));
}

int main(int argc, char **argv)
{
    tags = array_create(sizeof(data_tag));
	reset_tag();

    array files = array_create(sizeof(found_file));
    array filters = string_split("*.h");
    bool is_cancelled = false;
    platform_list_files_block(&files, "src\\", filters, true, 0, false, &is_cancelled, 0);

    for (s32 i = 0; i < files.length; i++)
    {
        found_file *file = array_at(&files, i);

        if (platform_file_exists(file->path))
        {
            printf("Parsing file: %s\n", file->path);
            parse_file(file->path);
        }
    }

    printf("tags: %d\n", tags.length);
    dump_html();

    return 0;
}