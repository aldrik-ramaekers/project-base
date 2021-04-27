#include <projectbase/project_base.h>

#define CONFIG_DIRECTORY "example_program_2"


ui_el_container* ui_main = 0;
void resize_ui2(platform_window* window, u32 change_x, u32 change_y)
{
	if (ui_main) ui_el_resize((ui_el_base*)ui_main, change_x, change_y);
}

void update_render_ui2(platform_window* window)
{
	if (!ui_main) {
		ui_main = ui_create(window);

		ui_el_container* container1 = ui_create_container(1, (ui_el_base*)ui_main);
		ui_el_layout* container2 = ui_create_layout(1, (ui_el_base*)ui_main, LAYOUT_VERTICAL);

		ui_el_container* container3 = ui_create_container(1, (ui_el_base*)container2);
		ui_el_layout* container4 = ui_create_layout(2, (ui_el_base*)container2, LAYOUT_HORIZONTAL);
		ui_el_container* container5 = ui_create_container(1, (ui_el_base*)container2);
		
		ui_el_container* container6 = ui_create_container(1, (ui_el_base*)container4);
		ui_el_container* container7 = ui_create_container(1, (ui_el_base*)container4);
		ui_el_container* container8 = ui_create_container(1, (ui_el_base*)container4);

		ui_print_tree((ui_el_base*)ui_main, 0);
	}

	ui_el_render((ui_el_base*)ui_main);
}

int main(int argc, char **argv)
{    
    platform_init(argc, argv, CONFIG_DIRECTORY);

    platform_window *window = platform_open_window("Hello",
                500, 500, 800, 600, 500, 500, update_render_ui2, resize_ui2);

    while(platform_keep_running(window)) {
        platform_handle_events();
    }

    settings_write_to_file();
    platform_destroy();

    return 0;
}