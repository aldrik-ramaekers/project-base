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
				qui_widget* sub1 = qui_create_toolbar_item_option_with_icon(toolbar_file, "Open File", "data/file.png");
				{
					qui_widget* sub2 = qui_create_toolbar_item_option(sub1, "Sub menu");
					{
						qui_create_toolbar_item_option(sub2, "Sub menu 2");
						qui_create_toolbar_item_option(sub2, "Sub menu 3");
					}
				}
				qui_create_toolbar_item_option(toolbar_file, "Create File");
				qui_create_toolbar_item_option_with_icon(toolbar_file, "Open Folder", "data/folder.png");
				qui_create_toolbar_item_option_with_icon(toolbar_file, "Reload", "data/undo.png");
			}

			qui_widget* toolbar_help = qui_create_toolbar_item(toolbar, "Help");
			{
				qui_create_toolbar_item_option(toolbar_help, "Website");
				qui_create_toolbar_item_option_with_icon(toolbar_help, "Support", "data/question.png");
			}
		}

		qui_widget* itembar = qui_create_itembar(layout);
		{
			qui_create_button_with_icon(itembar, "data/file.png");
			qui_create_toggle_button_with_icon(itembar, "data/folder.png");

			qui_create_itembar_separator(itembar);

			qui_create_button_with_icon(itembar, "data/undo.png");
			qui_create_button_with_icon(itembar, "data/redo.png");
			qui_create_button_with_icon(itembar, "data/play.png");
			qui_create_button_with_icon(itembar, "data/pause.png");

			qui_create_itembar_separator(itembar);

			qui_create_label(itembar, "Time display:");
			qui_create_button(itembar, "Milli");
		}

		qui_create_button(layout, "Hello!");
		qui_create_flex_container(layout, 1);

		qui_widget* bottom_resize_box = qui_create_size_container(layout, DIRECTION_TOP, 100);
		{
			qui_widget* layout1 = qui_create_horizontal_layout(bottom_resize_box);
			{
				qui_widget* box_left = qui_create_flex_container(layout1, 1);
				qui_flex_container_set_border(box_left, BORDER_RIGHT, 3);

				qui_widget* box_right = qui_create_flex_container(layout1, 1);
				qui_flex_container_set_border(box_right, BORDER_LEFT, 3);
			}
		}
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