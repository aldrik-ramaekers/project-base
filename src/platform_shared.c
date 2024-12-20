/* 
*  BSD 2-Clause “Simplified” License
*  Copyright (c) 2019, Aldrik Ramaekers, aldrik.ramaekers@protonmail.com
*  All rights reserved.
*/

void get_name_from_path(char *buffer, char *path)
{
	buffer[0] = 0;
	
	s32 len = strlen(path);
	if (len == 1)
	{
		return;
	}
	
	char *path_end = path + len;
#ifdef OS_LINUX
	while (*path_end != '/' && path_end >= path)
	{
		--path_end;
	}
#endif
#ifdef OS_WIN
	while (*path_end != '\\' && path_end >= path)
	{
		--path_end;
	}
#endif
	
	string_copyn(buffer, path_end+1, MAX_INPUT_LENGTH);
}

void get_directory_from_path(char *buffer, char *path)
{
	buffer[0] = 0;
	
	s32 len = strlen(path);
	if (len == 1)
	{
		return;
	}
	
	char *path_end = path + len;
#ifdef OS_LINUX
	while (*path_end != '/' && path_end >= path)
	{
		--path_end;
	}
#endif
#ifdef OS_WIN
	while (*path_end != '\\' && path_end >= path)
	{
		--path_end;
	}
#endif
	
	s32 offset = path_end - path;
	char ch = path[offset+1];
	path[offset+1] = 0;
	string_copyn(buffer, path, MAX_INPUT_LENGTH);
	path[offset+1] = ch;
}

void platform_autocomplete_path(char *buffer, bool want_dir)
{
	char dir[MAX_INPUT_LENGTH]; 
	char name[MAX_INPUT_LENGTH]; 
	get_directory_from_path(dir, buffer);
	get_name_from_path(name, buffer);
	
	// nothing to autocomplete
	if (name[0] == 0)
	{
		return;
	}
	
	// create filter
	string_appendn(name, "*", MAX_INPUT_LENGTH);
	
	array files = array_create(sizeof(found_file));
	array filters = get_filters(name);
	bool is_cancelled = false;
	platform_list_files_block(&files, dir, filters, false, 0, want_dir, &is_cancelled, 0);
	
	s32 index_to_take = -1;
	if (want_dir)
	{
		for (s32 i = 0; i < files.length; i++)
		{
			found_file *file = array_at(&files, i);
			
			if (platform_directory_exists(file->path))
			{
				index_to_take = i;
				break;
			}
		}
	}
	else
	{
		index_to_take = 0;
	}
	
	array_destroy(&filters);
	
	if (files.length > 0 && index_to_take != -1)
	{
		found_file *file = array_at(&files, index_to_take);
		string_copyn(buffer, file->path, MAX_INPUT_LENGTH);
	}
	
	for (s32 i = 0; i < files.length; i++)
	{
		found_file *match = array_at(&files, i);
		mem_free(match->matched_filter);
		mem_free(match->path);
	}
	array_destroy(&files);
}

array get_filters(char *pattern)
{
	array result = array_create(MAX_INPUT_LENGTH);
	
	char current_filter[MAX_INPUT_LENGTH];
	s32 filter_len = 0;
	while(*pattern)
	{
		char ch = *pattern;
		
		if (ch == ',')
		{
			current_filter[filter_len] = 0;
			array_push(&result, current_filter);
			filter_len = 0;
		}
		else
		{
			if(filter_len < MAX_INPUT_LENGTH-1)
			{
				current_filter[filter_len++] = ch;
			}
			else
			{
				current_filter[filter_len] = ch;
			}
		}
		
		pattern++;
	}
	current_filter[filter_len] = 0;
	array_push(&result, current_filter);
	
	return result;
}

void *platform_list_files_thread(void *args)
{
	list_file_args *info = args;
	
	array filters = get_filters(info->pattern);
	
	array *list = info->list;
	char *start_dir = info->start_dir;
	bool recursive = info->recursive;
	
	platform_list_files_block(info->list, info->start_dir, filters, info->recursive, info->bucket, info->include_directories, info->is_cancelled, info->info);
	
	mutex_lock(&info->list->mutex);
	//if (!(*info->is_cancelled))
	*(info->state) = true;
	mutex_unlock(&info->list->mutex);
	
	array_destroy(&filters);
	
	return 0;
}

void platform_list_files(array *list, char *start_dir, char *filter, bool recursive, memory_bucket *bucket, bool *is_cancelled, bool *state, search_info *info)
{
	list_file_args *args = memory_bucket_reserve(bucket, sizeof(list_file_args));
	args->list = list;
	args->start_dir = start_dir;
	args->pattern = filter;
	args->recursive = recursive;
	args->state = state;
	args->include_directories = 0;
	args->bucket = bucket;
	args->is_cancelled = is_cancelled;
	args->info = info;
	
	thread thr = thread_start(platform_list_files_thread, args);
	thread_detach(&thr);
}

void platform_open_file_dialog(file_dialog_type type, char *buffer, char *file_filter, char *start_path)
{
	struct open_dialog_args *args = mem_alloc(sizeof(struct open_dialog_args));
	args->buffer = buffer;
	args->type = type;
	args->file_filter = file_filter;
	args->start_path = start_path;
	
	thread thr;
	thr.valid = false;
	
	while (!thr.valid)
		thr = thread_start(platform_open_file_dialog_block, args);
	thread_detach(&thr);
}

void destroy_found_file_array(array *found_files)
{
	for (s32 i = 0; i < found_files->length; i++)
	{
		found_file *f = array_at(found_files, i);
		mem_free(f->matched_filter);
		mem_free(f->path);
	}
	array_destroy(found_files);
}

char *get_file_extension(char *path)
{
	while(*path != '.' && *path)
	{
		path++;
	}
	return path;
}

s32 filter_matches(array *filters, char *string, char **matched_filter)
{
	for (s32 i = 0; i < filters->length; i++)
	{
		char *filter = array_at(filters, i);
		if (string_match(filter, string))
		{
			*matched_filter = filter;
			return strlen(filter);
		}
	}
	return -1;
}
