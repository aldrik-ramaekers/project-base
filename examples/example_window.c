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
			qui_widget* time_dropdown = qui_create_dropdown(itembar);
			{
				qui_create_dropdown_option(time_dropdown, "Milliseconds");
				qui_create_dropdown_option(time_dropdown, "Seconds");
			}

			qui_create_button(itembar, "Run");
		}

		qui_widget* central_container = qui_create_flex_container(layout, 1);
		{
			qui_widget* horizontal_split = qui_create_horizontal_layout(central_container);
			{
				qui_widget* left_panel = qui_create_fixed_container(horizontal_split, 100);
				{
					(void)left_panel;
					// Tree
				}
				qui_widget* tabcontrol = qui_create_tabcontrol(horizontal_split);
				{
					qui_create_tabcontrol_panel(tabcontrol, "Data");
					qui_create_tabcontrol_panel(tabcontrol, "Options");
					qui_create_tabcontrol_panel(tabcontrol, "Display");
					qui_create_tabcontrol_panel(tabcontrol, "Appointments");
				}
			}
		}

		qui_widget* bottom_resize_box = qui_create_size_container(layout, DIRECTION_TOP, 100);
		{
			qui_widget* layout1 = qui_create_horizontal_layout(bottom_resize_box);
			{
				qui_widget* box_left = qui_create_flex_container(layout1, 1);
				{
					qui_widget* box_left_layout = qui_create_horizontal_layout(box_left);
					{
						// Table
					}		
				}

				qui_create_separator(layout1, 8);
				qui_widget* box_right = qui_create_flex_container(layout1, 1);
			}
		}
	}
}

void resize_ui(platform_window* window, u32 w, u32 h)
{
	qui_update(window, ui);
}

int main(int argc, char **argv)
{    
    platform_init(argc, argv, CONFIG_DIRECTORY);

	create_ui();
    window = platform_open_window("Hello!",
                700, 700, 1200, 900, 500, 500, update_render_ui2, resize_ui);


    while(platform_keep_running(window)) {
		window->do_draw = true;
        platform_handle_events();
    }

    settings_write_to_file();
    platform_destroy();

    return 0;
}