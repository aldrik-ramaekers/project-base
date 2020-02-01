/* 
*  BSD 2-Clause “Simplified” License
*  Copyright (c) 2019, Aldrik Ramaekers, aldrik.ramaekers@protonmail.com
*  All rights reserved.
*/

static void write_json_file(char *buffer, s32 length)
{
	cJSON *result = cJSON_CreateObject();
	if (cJSON_AddNumberToObject(result, "next_language_id", 
								global_language_id) == NULL)
		return;
	
	cJSON *langs = cJSON_AddArrayToObject(result, "languages");
	if (!langs) return;
	
	for (s32 i = 0; i < current_project->languages.length; i++)
	{
		cJSON *item = cJSON_CreateObject();
		language *lang = array_at(&current_project->languages, i);
		
		if (cJSON_AddStringToObject(item, "name", lang->name) == NULL)
			return;
		
		if (cJSON_AddNumberToObject(item, "id", lang->id) == NULL)
			return;
		
		cJSON_AddItemToArray(langs, item);
	}
	
	cJSON *terms = cJSON_AddArrayToObject(result, "terms");
	if (!terms) return;
	
	for (s32 i = 0; i < current_project->terms.length; i++)
	{
		cJSON *item = cJSON_CreateObject();
		term *trm = array_at(&current_project->terms, i);
		
		if (cJSON_AddStringToObject(item, "name", trm->name) == NULL)
			return;
		
		cJSON *translations = cJSON_AddArrayToObject(item, "translations");
		if (!translations) return;
		
		for (s32 x = 0; x < trm->translations.length; x++)
		{
			cJSON *sub_item = cJSON_CreateObject();
			translation *tr = array_at(&trm->translations, x);
			
			if (cJSON_AddNumberToObject(sub_item, "language_id", tr->language_id) == NULL)
				return;
			
			if (tr->value)
			{
				if (cJSON_AddStringToObject(sub_item, "value", tr->value) == NULL)
					return;
			}
			else
			{
				if (cJSON_AddNumberToObject(sub_item, "value", 0) == NULL)
					return;
			}
			
			cJSON_AddItemToArray(translations, sub_item);
		}
		
		cJSON_AddItemToArray(terms, item);
	}
	
	cJSON_PrintPreallocated(result, buffer, length, true);
	cJSON_Delete(result);
}

void save_project_to_file(char *path_buf)
{
	s32 size = ((current_project->terms.length+1)*
				(current_project->languages.length+1)*MAX_INPUT_LENGTH)*2;
	
	char *buffer = mem_alloc(size);
	memset(buffer, 0, size);
	
	char *file_extension = get_file_extension(path_buf);
	if (string_equals(file_extension, ".json") || string_equals(file_extension, ""))
	{
		write_json_file(buffer, size);
	}
	
	if (string_equals(file_extension, ""))
	{
		string_appendn(path_buf, ".json", MAX_INPUT_LENGTH);
	}
	
	platform_write_file_content(path_buf, "w", buffer, size);
}

static void *save_project_d(void *arg)
{
	char path_buf[MAX_INPUT_LENGTH];
	path_buf[0] = 0;
	
	char start_path[MAX_INPUT_LENGTH];
	snprintf(start_path, MAX_INPUT_LENGTH, "%s%s", binary_path, "");
	
	char default_save_file_extension[50];
	string_copyn(default_save_file_extension, "json", 50);
	
	struct open_dialog_args *args = mem_alloc(sizeof(struct open_dialog_args));
	args->buffer = path_buf;
	args->type = SAVE_FILE;
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
	save_project_to_file(path_buf);
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

static bool read_json_file(char *buffer, s32 size)
{
	cJSON *result = cJSON_Parse(buffer);
	if (!result) return false;
	
	cJSON *nlid = cJSON_GetObjectItem(result, "next_language_id");
	global_language_id = nlid->valueint;
	
	cJSON *language_list = cJSON_GetObjectItem(result, "languages");
	cJSON *lang;
	cJSON_ArrayForEach(lang, language_list)
	{
		language new_language;
		
		////
		cJSON *langid = cJSON_GetObjectItem(lang, "id");
		new_language.id = langid->valueint;
		
		////
        cJSON *langname = cJSON_GetObjectItem(lang, "name");
		new_language.name = mem_alloc(MAX_INPUT_LENGTH);
		string_copyn(new_language.name, langname->valuestring, MAX_INPUT_LENGTH);
		
		array_push(&current_project->languages, &new_language);
	}
	
	cJSON *term_list = cJSON_GetObjectItem(result, "terms");
	cJSON *term_dat;
	cJSON_ArrayForEach(term_dat, term_list)
	{
		term new_term;
		
		////
        cJSON *term_datname = cJSON_GetObjectItem(term_dat, "name");
		new_term.name = mem_alloc(MAX_INPUT_LENGTH);
		string_copyn(new_term.name, term_datname->valuestring, MAX_INPUT_LENGTH);
		
		new_term.translations = array_create(sizeof(translation));
		array_reserve(&new_term.translations, current_project->languages.length);
		
		cJSON *trans_list = cJSON_GetObjectItem(term_dat, "translations");
		cJSON *trans_dat;
		cJSON_ArrayForEach(trans_dat, trans_list)
		{
			translation new_translation;
			
			////
			cJSON *trans_datlanid = cJSON_GetObjectItem(trans_dat, "language_id");
			new_translation.language_id = trans_datlanid->valueint;
			
			////
			cJSON *trans_datname = cJSON_GetObjectItem(trans_dat, "value");
			new_translation.value = mem_alloc(MAX_INPUT_LENGTH);
			
			if (cJSON_IsString(trans_datname))
			{
				new_translation.value = mem_alloc(MAX_INPUT_LENGTH);
				string_copyn(new_translation.value, trans_datname->valuestring, MAX_INPUT_LENGTH);
			}
			else
				new_translation.value = 0;
			
			array_push(&new_term.translations, &new_translation);
		}
		
		array_push(&current_project->terms, &new_term);
	}
	
	return true;
}


void load_project_from_file(char *path_buf)
{
	char *file_extension = get_file_extension(path_buf);
	if (!string_equals(file_extension, ".json"))
	{
		platform_show_message(main_window, localize("invalid_search_result_file"), localize("error_importing_results"));
		return;
	}
	
	file_content content = platform_read_file_content(path_buf, "r");
	
	if (!content.content || content.file_error)
	{
		platform_destroy_file_content(&content);
		return;
	}
	
	if (string_equals(file_extension, ".json"))
	{
		bool result = read_json_file(content.content, content.content_length);
		if (!result) goto failed_to_load_file;
	}
	
	platform_destroy_file_content(&content);
	return;
	
	failed_to_load_file:
	platform_show_message(main_window, localize("invalid_search_result_file"), localize("error_importing_results"));
	platform_destroy_file_content(&content);
}

static void* load_project_d(void *arg)
{
	char path_buf[MAX_INPUT_LENGTH];
	path_buf[0] = 0;
	
	char start_path[MAX_INPUT_LENGTH];
	snprintf(start_path, MAX_INPUT_LENGTH, "%s%s", binary_path, "");
	
	char default_save_file_extension[50];
	string_copyn(default_save_file_extension, "json", 50);
	
	struct open_dialog_args *args = mem_alloc(sizeof(struct open_dialog_args));
	args->buffer = path_buf;
	args->type = OPEN_FILE;
	args->file_filter = SAVE_FILE_EXTENSION;
	args->start_path = start_path;
	args->default_save_file_extension = default_save_file_extension;
	
	platform_open_file_dialog_block(args);
	
	if (string_equals(path_buf, "")) return 0;
	if (!platform_file_exists(path_buf)) return 0;
	
	string_copyn(project_path, path_buf, MAX_INPUT_LENGTH);
	load_project_from_file(path_buf);
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