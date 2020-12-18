#include <projectbase/project_base.h>

#define CONFIG_DIRECTORY "example_program_1"

int main(int argc, char **argv)
{
    global_use_gpu = false;
    
    platform_init(argc, argv);
    settings_init(CONFIG_DIRECTORY);

    // localization_load();

    platform_window *window = platform_open_window("Example1", 
                500, 500, 800, 600, 500, 500);

    while(platform_keep_running(window)) {
        platform_handle_events(window);

		//platform_set_icon(&window, logo_small_img); // move to platform_open_window, put default icon in resource file

		if (window->do_draw) {
            ui_begin(1, window);
            {
                ui_begin_menu_bar();
                {
                    if (ui_push_menu("File"))
                    {
                        if (ui_push_menu_item("Poop", "Ctrl + O")) 
                        { 
                            push_notification("Poop!");

                            char buffer[4000];
                            platform_open_file_dialog(SAVE_FILE, buffer, 0, 0, "jpg");
                        }
                    }
                }
            }
            ui_end();

		    platform_window_swap_buffers(window);
        }
    }

    settings_write_to_file();

    platform_destroy_window(window);

    settings_destroy();
    platform_destroy();

    return 0;
}