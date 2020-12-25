/* 
*  BSD 2-Clause “Simplified” License
*  Copyright (c) 2019, Aldrik Ramaekers, aldrik.ramaekers@protonmail.com
*  All rights reserved.
*/

void platform_get_name_from_path(char *buffer, char *path)
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

void platform_get_directory_from_path(char *buffer, char *path)
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
	platform_get_directory_from_path(dir, buffer);
	platform_get_name_from_path(name, buffer);
	
	// nothing to autocomplete
	if (name[0] == 0)
	{
		return;
	}
	
	// create filter
	string_appendn(name, "*", MAX_INPUT_LENGTH);
	
	array files = array_create(sizeof(found_file));
	array filters = string_split(name);
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

void *platform_list_files_thread(void *args)
{
	list_file_args *info = args;
	
	array filters = string_split(info->pattern);
	platform_list_files_block(info->list, info->start_dir, filters, info->recursive, info->bucket, info->include_directories, info->is_cancelled, info->info);
	
	mutex_lock(&info->list->mutex);
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

void platform_open_file_dialog(file_dialog_type type, char *buffer, char *file_filter, char *start_path, char *save_file_extension)
{
	struct open_dialog_args *args = mem_alloc(sizeof(struct open_dialog_args));
	args->buffer = buffer;
	args->type = type;
	args->file_filter = file_filter;
	args->start_path = start_path;
	args->default_save_file_extension = save_file_extension;
	
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

char *platform_get_file_extension(char *path)
{
	while(*path != '.' && *path)
	{
		path++;
	}
	return path;
}

s32 platform_filter_matches(array *filters, char *string, char **matched_filter)
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

void platform_destroy_shared()
{
	localization_destroy();
	ui_destroy();
	assets_destroy();

	keyboard_input_destroy(&_global_keyboard);
}

void platform_init_shared(int argc, char **argv)
{
	_lib_loader_init();

	_global_keyboard = keyboard_input_create();
	_global_mouse = mouse_input_create();
	_global_camera = (camera){0.0f,0.0f,0.0f};

	// get fullpath of the directory the exe is residing in
	binary_path = platform_get_full_path(argv[0]);
	
	char buf[MAX_PATH_LENGTH];
	platform_get_directory_from_path(buf, binary_path);
	string_copyn(binary_path, buf, MAX_INPUT_LENGTH);

	assets_create();
	for (s32 i = 0; i < ASSET_WORKER_COUNT; i++) {
		thread asset_queue_worker_thread = thread_start(assets_queue_worker, NULL);
		thread_detach(&asset_queue_worker_thread);
	}
	ui_init(assets_load_font(_binary_src_resources_mono_ttf_start, _binary_src_resources_mono_ttf_end, 16));
	localization_init();
}

u64 __last_stamp = 0;
bool platform_keep_running(platform_window *window)
{
   u64 current_stamp = platform_get_time(TIME_FULL, TIME_US);
	u64 diff = current_stamp - __last_stamp;
	float diff_ms = diff / 1000.0f;
		
	if (diff_ms < TARGET_FRAMERATE)
	{
		double time_to_wait = (TARGET_FRAMERATE) - diff_ms;
		thread_sleep(time_to_wait*1000);
	}

	__last_stamp = platform_get_time(TIME_FULL, TIME_US);

	if (assets_do_post_process())
		window->do_draw = true;
		
	if (window->has_focus)
		window->do_draw = true;

	return window->is_open;
}

void _platform_register_window(platform_window* window) {
	if (!array_exists(&window_registry)) {
		window_registry = array_create(sizeof(platform_window*));
		array_reserve(&window_registry, 10);
	}

	array_push(&window_registry, &window);
}

void _platform_unregister_window(platform_window* window) {
	array_remove_by(&window_registry, &window);
}

void _switch_render_method(bool use_gpu) 
{
	global_use_gpu = use_gpu;

	for (s32 i = 0; i < window_registry.length; i++) {
		platform_window* w = *(platform_window**)array_at(&window_registry, i);
		platform_setup_backbuffer(w);
	}
	platform_setup_renderer();
	assets_switch_render_method();
}

void platform_handle_events()
{
	bool _use_gpu = settings_get_number_or_default("USE_GPU", 1);

	// USE_GPU setting changed outside of update loop.. change render method quickly.
	if (global_use_gpu != _use_gpu) {
		_switch_render_method(_use_gpu);
	}

	for (s32 i = 0; i < window_registry.length; i++) {
		platform_window* w = *(platform_window**)array_at(&window_registry, i);
		_platform_handle_events_for_window(w);

		if (w->do_draw) {
            w->update_func(w);
		    platform_window_swap_buffers(w);
        }

		if (!w->is_open) {
			platform_destroy_window(w);
		}
	}
}