#include <projectbase/project_base.h>

#define CONFIG_DIRECTORY "example_program_2"

platform_window *window;
qui ui;

void update_render_ui2(platform_window* window)
{
	qui_update(window, &ui);
	qui_render(window, &ui);
}

void create_ui() {
	ui = qui_setup();
	
	qui_create_button(&ui, "Hello!");

	qui_widget* toolbar = qui_create_toolbar(&ui);
	qui_create_toolbar_item(&qui, toolbar, "File");
}

int main(int argc, char **argv)
{    
    platform_init(argc, argv, CONFIG_DIRECTORY);

    window = platform_open_window("Hello!",
                500, 500, 900, 900, 300, 300, update_render_ui2, 0);

	create_ui();

    while(platform_keep_running(window)) {
        platform_handle_events();
    }

    settings_write_to_file();
    platform_destroy();

    return 0;
}