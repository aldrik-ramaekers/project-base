#include <projectbase/project_base.h>

#define CONFIG_DIRECTORY "example_program_1"

int main(int argc, char **argv)
{
    platform_init(argc, argv);
    settings_init(CONFIG_DIRECTORY);

    // localization_load();

    platform_window window = platform_open_window("Example1", 
                settings_get_number("WINDOW_WIDTH"), 
                settings_get_number("WINDOW_HEIGHT"), 0, 0, 800, 600);
    main_window = &window;

    while(window.is_open) {
        u64 last_stamp = platform_get_time(TIME_FULL, TIME_US); // make macro for this
		platform_handle_events(&window);		

		//platform_set_icon(&window, logo_small_img); // move to platform_open_window, put default icon in resource file
		
		if (assets_do_post_process())
			window.do_draw = true;
		
		if (window.has_focus) // move to handle_events()
			window.do_draw = true;
		
		if (window.do_draw) {

            ui_begin(1, &window);
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

        // make macro for this
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

    settings_destroy();
    platform_destroy();

    return 0;
}