#include <projectbase/project_base.h>

#define CONFIG_DIRECTORY "example_program_2"


ui_el_container* ui_main = 0;
void resize_ui2(platform_window* window, u32 change_x, u32 change_y)
{
	if (ui_main) {
		ui_el_resize((ui_el_base*)ui_main, change_x, change_y);
		// ui_print_tree((ui_el_base*)ui_main, 0);
	}
}

void update_render_ui2(platform_window* window)
{
	if (!ui_main) {
		ui_main = ui_create(window);

		ui_el_container* container1 = ui_create_container(el_size_h(1, 50, 150), (ui_el_base*)ui_main);
		ui_el_layout* layout1 = ui_create_layout(el_size(1), (ui_el_base*)ui_main, DIRECTION_HORIZONTAL);
		ui_el_container* container5 = ui_create_container(el_size_h(1, 50, 50), (ui_el_base*)ui_main);

		ui_el_container* container2 = ui_create_container(el_size(1), (ui_el_base*)layout1);
		ui_el_scrollable* container3 = ui_create_scrollable(el_size(1), (ui_el_base*)layout1);
		ui_el_container* container4 = ui_create_container(el_size(1), (ui_el_base*)layout1);

		ui_el_container* container9 = ui_create_container(el_size_h(1, 200,200), (ui_el_base*)container3);
		ui_el_container* container10 = ui_create_container(el_size_h(1, 200,200), (ui_el_base*)container3);
		ui_el_container* container11 = ui_create_container(el_size_h(1, 200,200), (ui_el_base*)container3);

		ui_el_container* container6 = ui_create_container(el_size(1), (ui_el_base*)container4);
		ui_el_container* container7 = ui_create_container(el_size(3), (ui_el_base*)container4);
		ui_el_container* container8 = ui_create_container(el_size(1), (ui_el_base*)container4);

		resize_ui2(window, 0,0);
	}

	ui_el_render((ui_el_base*)ui_main, window);

	//renderer->render_rectangle(0,0,500,500, rgb(255,0,0));
}

int main(int argc, char **argv)
{    
    platform_init(argc, argv, CONFIG_DIRECTORY);

    platform_window *window = platform_open_window("Hello",
                500, 500, 900, 900, 300, 300, update_render_ui2, resize_ui2);

    while(platform_keep_running(window)) {
        platform_handle_events();
    }

    settings_write_to_file();
    platform_destroy();

    return 0;
}