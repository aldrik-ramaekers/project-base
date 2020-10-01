#include <projectbase/project_base.h>

extern unsigned char _binary_examples_mono_ttf_start[];
extern unsigned char _binary_examples_mono_ttf_end[];

#define CONFIG_DIRECTORY "example_program_1"

font* main_font;

int main(int argc, char **argv)
{
    platform_init(argc, argv);
    settings_init(CONFIG_DIRECTORY);
    localization_init();

    // localization_load();

    platform_window window = platform_open_window("Example1", 
                settings_get_number("WINDOW_WIDTH"), 
                settings_get_number("WINDOW_HEIGHT"), 0, 0, 800, 600);
    main_window = &window;

    keyboard_input keyboard = keyboard_input_create();
	mouse_input mouse = mouse_input_create();

    camera camera;
	camera.x = 0;
	camera.y = 0;
	camera.rotation = 0;

    main_font = assets_load_font(_binary_examples_mono_ttf_start, _binary_examples_mono_ttf_end, 16);

    ui_create(&window, &keyboard, &mouse, &camera, main_font);

    while(window.is_open) {
        u64 last_stamp = platform_get_time(TIME_FULL, TIME_US);
		platform_handle_events(&window, &mouse, &keyboard);
		platform_set_cursor(&window, CURSOR_DEFAULT);
		
		platform_window_make_current(&window);
		//platform_set_icon(&window, logo_small_img);
		
		global_ui_context.layout.active_window = &window;
		global_ui_context.keyboard = &keyboard;
		global_ui_context.mouse = &mouse;
		
		if (assets_do_post_process())
			window.do_draw = true;
		
		if (window.has_focus)
			window.do_draw = true;
		
		if (window.do_draw) {

            window.do_draw = false;		
			render_clear(&window);
			camera_apply_transformations(&window, &camera);

            ui_begin(1);
            {
                ui_begin_menu_bar();
                {
                    if (ui_push_menu("File"))
                    {
                        if (ui_push_menu_item("Poop", "Ctrl + O")) 
                        { 
                            
                        }
                    }
                }
            }
            ui_end();

            update_render_notifications();			
		    platform_window_swap_buffers(&window);
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

    settings_write_to_file();

    platform_destroy_window(&window);

    ui_destroy();
    keyboard_input_destroy(&keyboard);

    localization_destroy();
    settings_destroy();
    platform_destroy();

    return 0;
}