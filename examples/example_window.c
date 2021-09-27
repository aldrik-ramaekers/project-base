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

	qui_widget* layout = qui_create_vertical_layout(ui);
	{
		qui_widget* toolbar = qui_create_toolbar(layout);
		{
			qui_widget* toolbar_file = qui_create_toolbar_item(toolbar, "File");
			{
				qui_widget* sub1 = qui_create_toolbar_item_option(toolbar_file, "Open File");
				{
					qui_create_toolbar_item_option(toolbar_file, "Create File");
					qui_create_toolbar_item_option(toolbar_file, "Find File");
					qui_create_toolbar_item_option(toolbar_file, "A Very Very Very Long Option");

					qui_widget* sub2 = qui_create_toolbar_item_option(sub1, "Sub menu");
					{
						qui_create_toolbar_item_option(sub2, "Sub menu 2");
						qui_create_toolbar_item_option(sub2, "Sub menu 3");
					}
				}
			}

			qui_create_toolbar_item(toolbar, "Help");
		}
		toolbar = qui_create_toolbar(layout);
		{
			
		}

		qui_create_button(layout, "Hello!");

		qui_create_fixed_container(layout, 100);
		qui_create_flex_container(layout, 1);
		qui_create_flex_container(layout, 2);
		qui_create_size_container(layout, TOP, 100);
	}
}

int main(int argc, char **argv)
{    
    platform_init(argc, argv, CONFIG_DIRECTORY);

    window = platform_open_window("Hello!",
                700, 700, 1200, 900, 500, 500, update_render_ui2, 0);

	create_ui();

    while(platform_keep_running(window)) {
		window->do_draw = true;
        platform_handle_events();
    }

    settings_write_to_file();
    platform_destroy();

    return 0;
}