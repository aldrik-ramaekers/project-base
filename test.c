#define CONFIG_DIRECTORY_LINUX "/.config/sdfgsdfgsdfg"
#define TARGET_FRAMERATE (1000/24.0)

#define _binary____data_translations_en_English_mo_start 999999
#define _binary____data_translations_en_English_mo_end 999999

#define _binary____data_translations_nl_Dutch_mo_start 999999
#define _binary____data_translations_nl_Dutch_mo_end 999999

#define _binary____data_imgs_nl_bmp_start 999999
#define _binary____data_imgs_nl_bmp_end 999999

#define _binary____data_imgs_en_bmp_start 999999
#define _binary____data_imgs_en_bmp_end 999999
#include "src/project_base.h"

// gcc -m64 -g test.c -o build/example1 -lX11 -lGL -lGLU -lXrandr -lm -lpthread -ldl

int main(int argc, char **argv)
{
	platform_init(argc, argv);
	char config_path_buffer[PATH_MAX];
	get_config_save_location(config_path_buffer);
	
	// load config
	settings_config config = settings_config_load_from_file(config_path_buffer);
	global_use_gpu = 1;

	platform_window window = platform_open_window("Text-search", 500, 500, 0, 0, 800, 600);
	main_window = &window;
	
#ifdef MODE_DEVELOPER
	startup_stamp = platform_get_time(TIME_FULL, TIME_US);
#endif
	
	//settings_page_create();
	
	debug_print_elapsed(startup_stamp, "settings page");
	
	// asset workers
	thread asset_queue_worker1 = thread_start(assets_queue_worker, NULL);
	thread asset_queue_worker2 = thread_start(assets_queue_worker, NULL);
	thread asset_queue_worker3 = thread_start(assets_queue_worker, NULL);
	thread_detach(&asset_queue_worker1);
	thread_detach(&asset_queue_worker2);
	thread_detach(&asset_queue_worker3);
	
	load_available_localizations();
	set_locale("en");
	
	keyboard_input keyboard = keyboard_input_create();
	mouse_input mouse = mouse_input_create();
	
	
	camera camera;
	camera.x = 0;
	camera.y = 0;
	camera.rotation = 0;
	
	ui_create(&window, &keyboard, &mouse, &camera, 0);
	
	//load_config(&config);
	
	while(window.is_open) {
		u64 last_stamp = platform_get_time(TIME_FULL, TIME_US);
		platform_handle_events(&window, &mouse, &keyboard);
		platform_set_cursor(&window, CURSOR_DEFAULT);
		
		platform_window_make_current(&window);
		
		global_ui_context.layout.active_window = &window;
		global_ui_context.keyboard = &keyboard;
		global_ui_context.mouse = &mouse;
		
		if (assets_do_post_process())
			window.do_draw = true;
		
		if (window.has_focus)
			window.do_draw = true;
		
		if (window.do_draw)
		{
#ifdef CHECKFPS
			u64 tmp1  = platform_get_time(TIME_FULL, TIME_US);
			static s32 total = 0;
			static s32 min = 0;
			static s32 max = 0;
			static s32 count = 0;
#endif
			
			window.do_draw = false;
			
			render_clear(&window);
			camera_apply_transformations(&window, &camera);
			
			global_ui_context.layout.width = global_ui_context.layout.active_window->width;
			// begin ui
			
			ui_begin(1);
			{
				
			}
			ui_end();
			// end ui
			
			update_render_notifications();
			
			platform_window_swap_buffers(&window);
			
#ifdef CHECKFPS
			{
				count++;
				u64 tmp2 = platform_get_time(TIME_FULL, TIME_US);
				u64 tmp3 = tmp2 - tmp1;
				if (tmp3 < max || max == 0) max = tmp3;
				if (tmp3 > min || min == 0) min = tmp3;
				total += tmp3;
				
				if (count % 24 == 0)
					printf("avg: %.2f, min: %.2f, max: %.2f\n", 1000000.0f/(total/count), 1000000.0f/min, 1000000.0f/max);
			}
#endif
		}
		
		u64 current_stamp = platform_get_time(TIME_FULL, TIME_US);
		u64 diff = current_stamp - last_stamp;
		float diff_ms = diff / 1000.0f;
		last_stamp = current_stamp;
		
		if (diff_ms < TARGET_FRAMERATE)
		{
			double time_to_wait = (TARGET_FRAMERATE) - diff_ms;
			thread_sleep(time_to_wait*1000);
		}
	}
	
	return 0;
}