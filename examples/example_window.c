#include <projectbase/project_base.h>

#define CONFIG_DIRECTORY "example_program_2"

void renderer_dropdown_change_callback(qui_widget* qui, char* text)
{
	if (strcmp(text, "CPU") == 0)
	{
		settings_set_number("USE_GPU", 0);
		printf("Renderer: CPU\n");
	}
	if (strcmp(text, "OpenGL") == 0)
	{
		settings_set_number("USE_GPU", 1);
		printf("Renderer: OpenGL\n");
	}
}

void theme_dropdown_change_callback(qui_widget* qui, char* text)
{
	if (strcmp(text, "Copy Platform Theme") == 0)
	{
		qui_set_theme(qui, 0, true);
		printf("Theme: Respecting platform theme\n");
	}
	if (strcmp(text, "Dark Theme") == 0)
	{
		qui_set_theme(qui, APPLICATION_THEME_DARK, false);
		printf("Theme: Dark mode\n");
	}
	if (strcmp(text, "Light Theme") == 0)
	{
		qui_set_theme(qui, APPLICATION_THEME_LIGHT, false);
		printf("Theme: Light mode\n");
	}
}

qui_widget* create_ui() {
	qui_widget* ui = qui_setup(500000); // 500kb

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

		qui_widget* itembar = qui_create_itembar_with_border(layout, BORDER_BOTTOM);
		{
			qui_create_button_with_icon(itembar, "data/file.png");
			qui_create_toggle_button_with_icon(itembar, "data/folder.png");

			qui_create_itembar_separator(itembar);

			qui_create_button_with_icon(itembar, "data/undo.png");
			qui_create_button_with_icon(itembar, "data/redo.png");
			qui_create_button_with_icon(itembar, "data/play.png");
			qui_create_button_with_icon(itembar, "data/pause.png");

			qui_create_itembar_separator(itembar);

			qui_create_label(itembar, "Theme:", false);
			qui_widget* theme_dropdown = qui_create_dropdown(itembar, theme_dropdown_change_callback);
			{
				qui_create_dropdown_option(theme_dropdown, "Copy Platform Theme");
				qui_create_dropdown_option(theme_dropdown, "Dark Theme");
				qui_create_dropdown_option(theme_dropdown, "Light Theme");
			}

			qui_create_label(itembar, "Renderer:", false);
			qui_widget* renderer_dropdown = qui_create_dropdown(itembar, renderer_dropdown_change_callback);
			{
				qui_create_dropdown_option(renderer_dropdown, "CPU");
				qui_create_dropdown_option(renderer_dropdown, "OpenGL");
			}

			qui_create_button(itembar, "Run");
		}

		qui_widget* central_container = qui_create_flex_container_s(layout, 1, 150);
		{
			qui_widget* horizontal_split = qui_create_horizontal_layout(central_container);
			{
				qui_widget* left_panel = qui_create_fixed_container(horizontal_split, 100);
				{
					qui_fixed_container_set_border(left_panel, BORDER_RIGHT, 2);
					(void)left_panel;
					// Tree
				}
				qui_widget* tabcontrol = qui_create_tabcontrol(horizontal_split);
				{
					qui_widget* tab_data_panel = qui_create_tabcontrol_panel(tabcontrol, "Data");
					{
						qui_widget* tab_layout = qui_create_vertical_layout(tab_data_panel);
						{							
							qui_create_fixed_container(tab_layout, 5);
							qui_widget* tab_itembar = qui_create_itembar(tab_layout);
							{
								qui_create_button(tab_itembar, "Refresh");
								qui_create_itembar_separator(tab_itembar);
								qui_create_button(tab_itembar, "Cancel");
								qui_create_button(tab_itembar, "Save Changes");
							}
							qui_create_fixed_container(tab_layout, 5);
							qui_widget* table = qui_create_table(tab_layout);
							{
								qui_widget* table_header = qui_create_table_row_header(table);
								{
									qui_create_table_row_entry(table_header, "Name");
									qui_create_table_row_entry(table_header, "Age");
									qui_create_table_row_entry(table_header, "Group");
									qui_create_table_row_entry(table_header, "Result");
								}

								for (s32 i = 0; i < 50; i++) {
									qui_widget* row = qui_create_table_row(table);
									{
										qui_create_table_row_entry(row, "2H₂ + O₂ ⇌ 2H₂O, R = 4.7 kΩ, ⌀ 200 mm");
										qui_create_table_row_entry(row, "Entry 2 Long");
										qui_create_table_row_entry(row, "Entry 3 Very long");

										qui_widget* btn_row = qui_create_table_row_entry(row, "");
										{
											qui_create_button(btn_row, "Edit");
											qui_create_button_with_icon(btn_row, "data/play.png");
										}
									}
								}
							}
						}
					}

					qui_widget* tab_plans_panel = qui_create_tabcontrol_panel(tabcontrol, "Plans");
					{
						qui_widget* tab_layout = qui_create_vertical_layout(tab_plans_panel);
						{
							qui_widget* title_container = qui_create_fixed_container(tab_layout, 50);
							{
								qui_fixed_container_set_border(title_container, BORDER_BOTTOM, 1);
								qui_widget* title_container_layout = qui_create_horizontal_layout(title_container);
								{
									qui_create_flex_container(title_container_layout, 1);
									qui_create_label(title_container_layout, "Subscription plans", false);
									qui_create_flex_container(title_container_layout, 1);
								}
							}
							qui_widget* subscription_container = qui_create_flex_container(tab_layout, 1);
							{
								qui_widget* plan_list_layout = qui_create_horizontal_layout(subscription_container);
								{
									for (s32 i = 0; i < 3; i++)
									{
										qui_widget* plan_container = qui_create_flex_container(plan_list_layout, 1);
										{
											// Create margin element here so we dont have to set margins manually.
											qui_flex_container_set_border(plan_container, BORDER_TOP|BORDER_RIGHT|BORDER_BOTTOM|BORDER_LEFT, 1);
											plan_container->margin_x = 10;
											plan_container->margin_y = 10;
											qui_widget* plan_layout = qui_create_vertical_layout(plan_container);
											{
												qui_create_label(plan_layout, "Plan 1", false);
											}
										}
									}
								}
							}
						}
					}

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
						qui_widget* scroll = qui_create_vertical_scroll(box_left_layout);
						{
							qui_widget* tab_itembar = qui_create_itembar_with_border(scroll, BORDER_BOTTOM);
							{
								qui_create_button(tab_itembar, "Clear");
							}
							qui_create_label(scroll, "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor", true);
							qui_create_label(scroll, "Sed ut perspiciatis unde", true);
							qui_create_label(scroll, "At vero eos et accusamus et iusto odio dignissimos ducimus qui blanditiis "
								"praesentium voluptatum deleniti atque corrupti quos dolores et quas molestias excepturi sint "
								"occaecati cupiditate non provident, similique sunt in culpa qui officia deserunt mollitia animi, id est laborum et dolorum fuga.", true);
						}
					}
				}

				qui_create_separator(layout1, 8);
				qui_widget* box_right = qui_create_flex_container(layout1, 1);
			}
		}
	}

	return ui;
}

font* def_font;
void update_func(platform_window* window)
{
	if (!def_font->loaded) return;
	renderer->render_clear(window, rgb(0,0,0));

	int columns = 60;

	for (int i = 8322; i < 9000; i++)
	{
		int idx = i - 8322;
		int y = idx / columns;
		int x = idx % columns;

		glyph g = assets_get_glyph(def_font, i);
		renderer->render_glyph(def_font, x * 25, y * 25, g, rgb(255,255,255));
	}
}

int main(int argc, char **argv)
{    
    platform_init(argc, argv, CONFIG_DIRECTORY);

    platform_window *window = platform_open_window_gui("Hello!", 700, 700, 9999, 9999, 500, 500, create_ui());

	//platform_window *window = platform_open_window("test", 1400, 900, 1400, 900, 1400, 900, update_func,0,0);
	//def_font = assets_load_font(noto_regular_ttf, noto_regular_ttf+noto_regular_ttf_len, 20);

	//popup_window_show("Test Popup Message", "You are a stinky poopy.", POPUP_BTN_OK|POPUP_BTN_CANCEL, POPUP_TYPE_INFO);
	//popup_window_show("Booba", "test123", POPUP_BTN_OK|POPUP_BTN_CANCEL, POPUP_TYPE_WARNING);
	//popup_window_show("Booba", "test123", POPUP_BTN_OK|POPUP_BTN_CANCEL, POPUP_TYPE_DEFAULT);

    while(platform_keep_running(window)) {
        platform_handle_events();
    }

    settings_write_to_file();
    platform_destroy();

	memory_print_leaks();

    return 0;
}