#include <projectbase/project_base.h>

#define CONFIG_DIRECTORY "example_program_2"

void update_render_ui2(platform_window* window)
{

}

int main(int argc, char **argv)
{    
    platform_init(argc, argv, CONFIG_DIRECTORY);

    platform_window *window = platform_open_window("Hello",
                500, 500, 900, 900, 300, 300, update_render_ui2, 0);

    while(platform_keep_running(window)) {
        platform_handle_events();
    }

    settings_write_to_file();
    platform_destroy();

    return 0;
}