/* 
*  BSD 2-Clause “Simplified” License
*  Copyright (c) 2019, Aldrik Ramaekers, aldrik.ramaekers@protonmail.com
*  All rights reserved.
*/

void start_new_project();
s32 add_language_to_project(char *buffer);

s32 write_mo_file(char *buffer, s32 buffer_size, s32 language_id)
{
	s32 identifier_table_size = current_project->terms.length * 8;
	s32 translation_table_size = current_project->terms.length * 8;
	
	s32 size_written = 0;
	
	mo_header header;
	header.magic_number = 0x950412de;
	header.file_format_revision = 1;
	header.number_of_strings = current_project->terms.length;
	header.identifier_table_offset = 0x40; // keep empty bytes (0x2-0x4) for format changes
	header.translation_table_offset = header.identifier_table_offset + identifier_table_size;
	header.hashtable_size = 0;
	header.hashtable_offset = header.translation_table_offset + translation_table_size;
	
	s32 current_data_write_offset = header.hashtable_offset+header.hashtable_size;
	
	// write identifier table and data
	mo_entry *identifier_table = (mo_entry*)(buffer + header.identifier_table_offset);
	for (s32 i = 0; i < current_project->terms.length; i++)
	{
		term *t = array_at(&current_project->terms, i);
		
		mo_entry new_identifier_entry;
		new_identifier_entry.length = strlen(t->name);
		new_identifier_entry.offset = current_data_write_offset;
		
		memcpy(buffer+current_data_write_offset, t->name, new_identifier_entry.length);
		current_data_write_offset += new_identifier_entry.length+1;
		identifier_table[i] = new_identifier_entry;
	}
	
	// write translation table and data
	mo_entry *translation_table = (mo_entry*)(buffer + header.translation_table_offset);
	for (s32 i = 0; i < current_project->terms.length; i++)
	{
		term *t = array_at(&current_project->terms, i);
		
		mo_entry new_translation_entry;
		
		for (s32 x = 0; x < t->translations.length; x++)
		{
			translation *tr = array_at(&t->translations, x);
			
			if (tr->language_id == language_id)
			{
				new_translation_entry.length = strlen(tr->value);
				new_translation_entry.offset = current_data_write_offset;
				
				memcpy(buffer+current_data_write_offset, tr->value, new_translation_entry.length);
				current_data_write_offset += new_translation_entry.length+1;
				break;
			}
		}
		
		translation_table[i] = new_translation_entry;
	}
	
	size_written = current_data_write_offset;
	
	memcpy(buffer, &header, sizeof(mo_header));
	return size_written;
}

void save_project_to_folder(char *path_buf)
{
	for (s32 i = 0; i < current_project->languages.length; i++)
	{
		language *lang = array_at(&current_project->languages, i);
		
		s32 size = ((current_project->terms.length+1)*MAX_INPUT_LENGTH)*2;
		
		char *buffer = mem_alloc(size);
		memset(buffer, 0, size);
		
		s32 size_written = write_mo_file(buffer, size, lang->id);
		
		char complete_path_buf[MAX_INPUT_LENGTH];
#ifdef OS_WIN
		sprintf(complete_path_buf, "%s\\%s.mo", path_buf, lang->name);
#endif
#ifdef OS_LINUX
		sprintf(complete_path_buf, "%s/%s.mo", path_buf, lang->name);
#endif
		
		platform_write_file_content(complete_path_buf, "wb", buffer, size_written);
		mem_free(buffer);
	}
}

static void *save_project_d(void *arg)
{
	char path_buf[MAX_INPUT_LENGTH];
	path_buf[0] = 0;
	
	char start_path[MAX_INPUT_LENGTH];
	snprintf(start_path, MAX_INPUT_LENGTH, "%s%s", binary_path, "");
	
	char default_save_file_extension[50];
	string_copyn(default_save_file_extension, "", 50);
	
	struct open_dialog_args *args = mem_alloc(sizeof(struct open_dialog_args));
	args->buffer = path_buf;
	args->type = OPEN_DIRECTORY;
	args->file_filter = SAVE_FILE_EXTENSION;
	args->start_path = start_path;
	args->default_save_file_extension = default_save_file_extension;
	
	platform_open_file_dialog_block(args);
	
	char tmp_dir_buffer[MAX_INPUT_LENGTH];
	get_directory_from_path(tmp_dir_buffer, path_buf);
	
	char tmp_name_buffer[MAX_INPUT_LENGTH];
	get_name_from_path(tmp_name_buffer, path_buf);
	
	if (string_equals(path_buf, "")) return 0;
	if (string_equals(tmp_name_buffer, "")) return 0;
	if (!platform_directory_exists(tmp_dir_buffer)) return 0;
	
	string_copyn(project_path, path_buf, MAX_INPUT_LENGTH);
	save_project_to_folder(path_buf);
	return 0;
}

void save_project()
{
	thread thr;
	thr.valid = false;
	
	while (!thr.valid)
		thr = thread_start(save_project_d, 0);
	
	thread_detach(&thr);
}

bool read_mo_file(char *buffer, s32 buffer_size, s32 language_id)
{
	mo_header header = *((mo_header*)buffer);
	
	mo_entry *identifiers = (mo_entry*)(buffer + header.identifier_table_offset);
	mo_entry *translations = (mo_entry*)(buffer + header.translation_table_offset);
	
	if (current_project->terms.length == 0)
	{
		for (s32 i = 0; i < header.number_of_strings; i++)
		{
			term t;
			t.name = mem_alloc(MAX_TERM_NAME_LENGTH);
			t.translations = array_create(sizeof(translation));
			array_reserve(&t.translations, current_project->languages.length);
			
			string_copyn(t.name, buffer+identifiers[i].offset, MAX_INPUT_LENGTH);
			array_push(&current_project->terms, &t);
		}
	}
	else if (current_project->terms.length != header.number_of_strings)
	{
		// TODO(Aldrik): localize
		platform_show_message(main_window, "Warning", "File is missing terms");
	}
	
	for (s32 i = 0; i < current_project->terms.length; i++)
	{
		term *t = array_at(&current_project->terms, i);
		
		translation tr;
		tr.value = mem_alloc(MAX_INPUT_LENGTH);
		string_copyn(tr.value, buffer+translations[i].offset, MAX_INPUT_LENGTH);
		tr.language_id = language_id;
		array_push(&t->translations, &tr);
	}
	
	return true;
}


void load_project_from_folder(char *path_buf)
{
	if (!platform_directory_exists(path_buf))
	{
		// TODO(Aldrik): localize
		platform_show_message(main_window, "Error loading project", "Project directory does not exist");
		return;
	}
	
	start_new_project();
	
	array files = array_create(sizeof(found_file));
	array filters = get_filters("*.mo");
	
	bool is_cancelled = false;
	
	char total_path[MAX_INPUT_LENGTH];
	total_path[0] = 0;
	string_copyn(total_path, path_buf, MAX_INPUT_LENGTH);
	
#ifdef OS_WIN
	string_appendn(total_path, "\\", MAX_INPUT_LENGTH);
#endif
#ifdef OS_LINUX
	string_appendn(total_path, "/", MAX_INPUT_LENGTH);
#endif
	
	platform_list_files_block(&files, total_path, filters, false, 0, false, &is_cancelled);
	for (s32 i = 0; i < files.length; i++)
	{
		found_file *file = array_at(&files, i);
		
		file_content content = platform_read_file_content(file->path, "rb");
		if (content.content && !content.file_error)
		{
			char name[MAX_INPUT_LENGTH];
			get_name_from_path(name, file->path);
			name[strlen(name)-3] = 0; // remove .mo extension
			
			language l;
			l.name = mem_alloc(MAX_INPUT_LENGTH);
			string_copyn(l.name, name, MAX_INPUT_LENGTH);
			l.id = global_language_id++;
			array_push(&current_project->languages, &l);
			
			read_mo_file(content.content, content.content_length, l.id);
		}
		platform_destroy_file_content(&content);
	}
	array_destroy(&filters);
	
	for (s32 i = 0; i < files.length; i++)
	{
		found_file *match = array_at(&files, i);
		mem_free(match->matched_filter);
		mem_free(match->path);
	}
	array_destroy(&files);
}

static void* load_project_d(void *arg)
{
	char path_buf[MAX_INPUT_LENGTH];
	path_buf[0] = 0;
	
	char start_path[MAX_INPUT_LENGTH];
	snprintf(start_path, MAX_INPUT_LENGTH, "%s%s", binary_path, "");
	
	char default_save_file_extension[50];
	string_copyn(default_save_file_extension, "", 50);
	
	struct open_dialog_args *args = mem_alloc(sizeof(struct open_dialog_args));
	args->buffer = path_buf;
	args->type = OPEN_DIRECTORY;
	args->file_filter = SAVE_FILE_EXTENSION;
	args->start_path = start_path;
	args->default_save_file_extension = default_save_file_extension;
	
	platform_open_file_dialog_block(args);
	
	if (string_equals(path_buf, "")) return 0;
	if (!platform_file_exists(path_buf)) return 0;
	
	string_copyn(project_path, path_buf, MAX_INPUT_LENGTH);
	load_project_from_folder(path_buf);
	return 0;
}

void load_project()
{
	thread thr;
	thr.valid = false;
	
	while (!thr.valid)
		thr = thread_start(load_project_d, 0);
	thread_detach(&thr);
}