#include <projectbase/project_base.h>

#define CONFIG_DIRECTORY "example_program_2"

platform_window *window;
qui_widget* ui;

void update_render_ui2(platform_window* window)
{
	qui_update(window, ui);
	qui_render(window, ui);
}

void create_ui() {
	ui = qui_setup();

	qui_create_button(ui, "Hello!");

	qui_widget* toolbar = qui_create_toolbar(ui);
	qui_widget* toolbar_file = qui_create_toolbar_item(toolbar, "File");
	qui_create_toolbar_item(toolbar, "Help");

	qui_widget* sub1 = qui_create_toolbar_item_option(toolbar_file, "Open File");
	qui_create_toolbar_item_option(toolbar_file, "Create File");
	qui_create_toolbar_item_option(toolbar_file, "Find File");
	qui_widget* sub2 = qui_create_toolbar_item_option(sub1, "Sub menu");
	qui_create_toolbar_item_option(sub2, "Sub menu 2");
	qui_create_toolbar_item_option(sub2, "Sub menu 3");
}

int main(int argc, char **argv)
{    
    platform_init(argc, argv, CONFIG_DIRECTORY);

    window = platform_open_window("Hello!",
                900, 900, 900, 900, 900, 900, update_render_ui2, 0);

	create_ui();

    while(platform_keep_running(window)) {
		window->do_draw = true;
        platform_handle_events();
    }

    settings_write_to_file();
    platform_destroy();

    return 0;
}