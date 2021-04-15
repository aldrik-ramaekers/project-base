#include <projectbase/project_base.h>

/*

# Format
- declarations start with // followed by 1 tab.
- identifiers are prefixed with :/.
- identifiers are postfixed with 1 tab.
- multi line declarations are prefixed with 3 tabs after the first line.
- lists items can be defined with a prefixed '-'

Example:
//	:/Title Hello world!
//	:Text	Multi
//			Line
//			String

*/

#define IDENTIFIER_CONTINUATION 	"//			"
#define IDENTIFIER_PREFIX 			"//	:/"
#define INFO_IDENTIFIER 			"//	:/Info"
#define RETURN_IDENTIFIER 			"//	:/Ret"
#define TITLE_IDENTIFIER 			"//	:/Title"
#define TEXT_IDENTIFIER 			"//	:/Text"

typedef enum t_parse_state
{
	PARSING_SEARCHING,
	PARSING_TITLE,
	PARSING_INFO,
	PARSING_TEXT,
	PARSING_RET,
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
		int new_len = strlen(str) + strlen(append) + 5;
		char *newbuf = mem_alloc(new_len);
		memset(newbuf, 0, new_len);
		strcpy(newbuf, str);
		if (append[0] == '-') strcat(newbuf, "<br>");
		strcat(newbuf, append);
	}
	else
	{
		return append;
	}
}

char* str_remove_excess_indent(char* str)
{
	int len = strlen(str) + 1;
	char* buf = mem_alloc(len);
	memset(buf, 0, len);

	int cursor = 0;
	char last = 0;
	while(*str)
	{
		if (!((*str == ' ' || *str == '	') && (last == ' ' || last == '	'))) buf[cursor++] = *str;
		last = *str;
		str++;
	}
	return buf;
}

bool str_is_func_def(char* str)
{
    return string_contains(str, "* *(*);") || string_contains(str, "* *();");
}

#define IS_CONTINUATION(_ident) (string_contains(string, _ident) || string_contains(string, IDENTIFIER_CONTINUATION)) 
parse_state current_state = PARSING_SEARCHING;

void parse_chapter(char *string)
{
	log_assert(current_state == PARSING_TITLE || current_state == PARSING_TEXT, "invalid state for parsing chapter");

	if (current_state == PARSING_TITLE)
	{
		if (!IS_CONTINUATION(TITLE_IDENTIFIER)) { 
			current_state = PARSING_SEARCHING;
			if (!string_contains(string, TEXT_IDENTIFIER)) store_tag();
		}
		else current_tag.title = str_append_newline(current_tag.title, get_str_after_tab(string));
	}
	if (current_state == PARSING_TEXT)
	{
		if (!IS_CONTINUATION(TEXT_IDENTIFIER)) { current_state = PARSING_SEARCHING; store_tag(); }
		else current_tag.text = str_append_newline(current_tag.text, get_str_after_tab(string));
	}
}

void parse_function(char *string)
{
	log_assert(current_state == PARSING_INFO || current_state == PARSING_RET, "invalid state for parsing function");

	if (str_is_func_def(string))
	{
		current_tag.func = str_append_newline(current_tag.func, str_remove_excess_indent(string));
		current_state = PARSING_SEARCHING;
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

void parse_line(char *string)
{
    int str_len = strlen(string);

	if (string_contains(string, INFO_IDENTIFIER)) { current_state = PARSING_INFO; set_tag_type(FUNCTION); };
	if (string_contains(string, RETURN_IDENTIFIER)) current_state = PARSING_RET;

	if (string_contains(string, TITLE_IDENTIFIER)) { current_state = PARSING_TITLE; set_tag_type(CHAPTER); };
	if (string_contains(string, TEXT_IDENTIFIER)) { current_state = PARSING_TEXT; };

	if (current_tag.type == FUNCTION) parse_function(string);
	if (current_tag.type == CHAPTER) parse_chapter(string);
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

#define DOCUMENT_TITLE "Project-base Technical Reference Manual"

#define PAGE_BREAK "<div style=\"page-break-before:always;\"></div>"
#define STYLE_TOPBAR "style=\"\""
#define STYLE_TOPBAR_SUB "style=\"font-size:0.6em;float:right;\""
#define STYLE_TOPBAR_TITLE "style=\"font-size:0.7em;flex: 3;margin-bottom:-17px;\""
#define STYLE_DOCUMENT "style=\"margin-left: -15%;width: 130%;text-align: justify; text-justify: inter-word; font-family: 'Roboto Condensed', sans-serif;\""
#define STYLE_HEADING1 "style=\"font-size:1.1em;\""
#define STYLE_HEADING2 "style=\"font-size:1.0em;\""
#define STYLE_HEADING3 "style=\"font-size:0.9em;\""
#define STYLE_TEXT "style=\"font-size:0.9em;\""
#define STYLE_SEPARATOR "style=\"width: 100%;background-color:rgb(0, 98, 208);padding:0px;margin:0px;color:white;padding-left: 5px;font-style: italic;\""
#define STYLE_SEPARATOR_SMALL "style=\"width: 100%;height:5px;background-color:rgb(0, 98, 208);padding:0px;margin:0px;color:white;padding-left: 5px;font-style: italic;margin-top: 20px;\""
#define STYLE_FUNCTION_CONTAINER "style=\"padding-bottom: 20px;\""
#define STYLE_FUNCTION_DEF "style=\"padding: 5px;background-color:rgb(243, 243, 248);border-radius:3px;border:2px solid rgb(221, 221, 221);\""

int count_functions_for_chapter(int index)
{
	int count = 0;
	for (int i = index; i < tags.length; i++)
    {
        data_tag* tag = array_at(&tags, i);

		if (tag->type == FUNCTION) {
			count++;
		}
		else {
			break;
		}
	}
	return count;
}

char* dump_functions_for_chapter(char* body, int index, int current_chapter_nr)
{
	int current_function_nr = 1;
    for (int i = index; i < tags.length; i++)
    {
        data_tag* tag = array_at(&tags, i);

		if (tag->type == FUNCTION) {
			APPEND(body, "<div "STYLE_FUNCTION_CONTAINER">");
						
			APPEND(body, "<h3 "STYLE_HEADING3">");
			char id[20];
			sprintf(id, "%d.1.%d	", current_chapter_nr, current_function_nr);
			APPEND(body, id);
			APPEND(body, "[FUNCTION NAME]");
			APPEND(body, "</h3>");

			APPEND(body, "<div "STYLE_FUNCTION_DEF">");
			APPEND(body, tag->func);
			APPEND(body, "</div>");

			APPEND(body, "<br>");
			APPEND(body, "<div "STYLE_TEXT">");
			APPEND(body, tag->info);
			APPEND(body, "</div>");

			if (tag->ret) {
				APPEND(body, "<div "STYLE_TEXT">");
				APPEND(body, "<b>Returns: </b>");
				APPEND(body, tag->ret);
				APPEND(body, "</div>");
			}

			APPEND(body, "</div>");
			current_function_nr++;
		}
		else {
			break;
		}
	}
	return body;
}

void dump_html()
{
    char* header =
    "<html>"
    "<body>"
	"<head>"
	"<link rel=\"preconnect\" href=\"https://fonts.gstatic.com\"> <link href=\"https://fonts.googleapis.com/css2?family=Roboto+Condensed:ital,wght@0,300;0,400;0,700;1,300;1,400;1,700&display=swap\" rel=\"stylesheet\">"
	"</head>"

	"<div "STYLE_DOCUMENT">"
    	"<h1>"DOCUMENT_TITLE"</h1>"
		"<div "STYLE_SEPARATOR">For Version 2.0.0</div>"

		"<h1 "STYLE_HEADING1">1	About</h1>"
		"<div "STYLE_TEXT">Written by Aldrik Ramaekers, aldrik.ramaekers@protonmail.com.<br>This document is distributed under the BSD 2-Clause 'Simplified' License.</div>"

		"<h1 "STYLE_HEADING1">2	Introduction</h1>"
		"<div "STYLE_TEXT">This document gives a technical description for the Project-base library.The Project-base library is a general purpose library intended for creating graphical programs for the Windows and Linux operating system.This document describes all the components"
		"of the Project-base library and gives examples for using these components.</div>"
	"</div>"
    "</body>"
    "</html>";

    char* body = mem_alloc(MAX_BODY_LEN);
    memset(body, 0, MAX_BODY_LEN);

    APPEND(body, "<!DOCTYPE html>");
	APPEND(body, "<html lang='en'>");
	APPEND(body, "<head>");
	APPEND(body, "<meta charset='utf-8' />");
	APPEND(body, "</head>");
    APPEND(body, "<body style=\"color: red;\">");

	int current_chapter_nr = 0;
	int current_function_nr = 1;
    for (int i = 0; i < tags.length; i++)
    {
        data_tag* tag = array_at(&tags, i);

        if (tag->type == CHAPTER)
        {
			current_function_nr = 1;
			current_chapter_nr++;

			APPEND(body, PAGE_BREAK);
			APPEND(body, "</div>");
			APPEND(body, "<div "STYLE_DOCUMENT">");

			APPEND(body, "<div "STYLE_TOPBAR">");
				APPEND(body, "<div "STYLE_TOPBAR_SUB"></div>");
				APPEND(body, "<div "STYLE_TOPBAR_TITLE">"DOCUMENT_TITLE"</div>");
				APPEND(body, "<div "STYLE_TOPBAR_SUB">For Version 2.0.0</div>");
				APPEND(body, "<div "STYLE_SEPARATOR_SMALL"></div>");
			APPEND(body, "</div>");

            APPEND(body, "<h1 "STYLE_HEADING1">");
			char id[20];
			sprintf(id, "%d	", current_chapter_nr);
			APPEND(body, id);
            APPEND(body, tag->title);
            APPEND(body, "</h1>");
			APPEND(body, "<div "STYLE_TEXT">");
            APPEND(body, tag->text);
            APPEND(body, "</div>");

			// Function sub-chapter
			if (count_functions_for_chapter(i+1)) {
				APPEND(body, "<h2 "STYLE_HEADING2">");
				sprintf(id, "%d.%d	", current_chapter_nr, current_function_nr);
				APPEND(body, id);
				APPEND(body, "Functions");
				APPEND(body, "</h2>");

				dump_functions_for_chapter(body, i+1, current_chapter_nr);
			}
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
            parse_file(file->path);
        }
    }

    dump_html();

    return 0;
}