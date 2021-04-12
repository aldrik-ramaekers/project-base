#include <projectbase/project_base.h>

/*

# Assumptions
    - function declarations are single line.
    - Text blocks are 3 tabs indented
    - function tags are 1 tab indented and prefixed with :/[tag]

*/

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
    current_tag.text = 0;
    current_tag.ret = 0;
    current_tag.info = 0;
    current_tag.title = 0;
    current_tag.type = 0;
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
    int total_len = strlen(str) + strlen(append) + 2;

    char *newbuf = mem_alloc(total_len);
    strcpy(newbuf, str);
    strcat(newbuf, "\n");
    strcat(newbuf, append);
}

bool str_is_func_def(char* str)
{
    return string_contains(str, "* *(*);");
}

bool last_line_was_text = false; // take care of multi-line text blocks.
void parse_line(char *string)
{
    int str_len = strlen(string);
    if (str_len <= 5) return;

    if (string_contains(string, "// :/Info"))
    {
        set_tag_type(FUNCTION);
        current_tag.info = get_str_after_tab(string);
    }
    if (string_contains(string, "// :/Return"))
    {
        set_tag_type(FUNCTION);
        current_tag.ret = get_str_after_tab(string);
    }
    if (current_tag.type == FUNCTION && str_is_func_def(string))
    {
        current_tag.func = string;
    }
    if (string_contains(string, "   :/Title"))
    {
        set_tag_type(CHAPTER);
        current_tag.title = get_str_after_tab(string);
    }
    
    if (string_contains(string, "   :/Text"))
    {
        set_tag_type(CHAPTER);
        current_tag.text = get_str_after_tab(string);
        last_line_was_text = true;
    }
    else if (string_contains(string, "           ") && last_line_was_text)
    {
        set_tag_type(CHAPTER);
        current_tag.text = str_append_newline(current_tag.text, get_str_after_tab(string));
        last_line_was_text = true;
    }
    else if (last_line_was_text)
    {
        store_tag();
        last_line_was_text = false;
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

int main(int argc, char **argv)
{
    tags = array_create(sizeof(data_tag));

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

    return 0;
}