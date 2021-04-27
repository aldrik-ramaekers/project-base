#include <projectbase/project_base.h>

#define CONFIG_DIRECTORY "example_program_1"

extern unsigned char _binary_examples_en_mo_start[];
extern unsigned char _binary_examples_en_mo_end[];

extern unsigned char _binary_examples_logo_64_bmp_start[];
extern unsigned char _binary_examples_logo_64_bmp_end[];

extern unsigned char _binary_examples_logo_64_png_start[];
extern unsigned char _binary_examples_logo_64_png_end[];

extern unsigned char _binary_examples_mono_ttf_start[];
extern unsigned char _binary_examples_mono_ttf_end[];

void update_render_sub(platform_window* window)
{
     ui_begin(window);
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
            if (ui_push_menu("Menu"))
            {
                if (ui_push_menu_item("New Window", "Ctrl + O")) 
                { 
                    
                }
            }
        }
    }
    ui_end();
}

image* img;
image* img2;
image* img3;
image* img4;

font* font_big;
font* font_very_big;

void update_render_main(platform_window* window) 
{
    ui_begin(window);
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
            if (ui_push_menu("Menu"))
            {
                if (ui_push_menu_item("New Window", "Ctrl + O")) 
                { 
                    platform_window *s = platform_open_window(localize("window_title_sub"), 500, 500, 800, 600, 500, 500, update_render_sub, 0);
                }
            }
        }
    }
    ui_end();

	renderer->render_image(img, 0, 0, 50, 50);
	renderer->render_image(img2, 0, 50, 50, 50);
	renderer->render_image(img3, 0, 100, 50, 50);
	renderer->render_image(img3, 0, 150, 50, 50);

	renderer->render_text(font_big, 120, 150, "BAZINGA!", rgb(255,100,100));
	renderer->render_text(font_very_big, 120, 180, "BIG BAZINGA!", rgb(100,255,100));
	
}

int main(int argc, char **argv)
{    
    platform_init(argc, argv, CONFIG_DIRECTORY);

    localization_load(_binary_examples_en_mo_start,
                      _binary_examples_en_mo_end,
                      0,0, "en", "English");

    localization_set_locale("en");

	img = assets_load_bitmap(_binary_examples_logo_64_bmp_start, _binary_examples_logo_64_bmp_end);
	img2 = assets_load_image(_binary_examples_logo_64_png_start, _binary_examples_logo_64_png_end);
	img3 = assets_load_bitmap_from_file("logo_64.bmp");
	img4 = assets_load_image_from_file("logo_64.png");

	font_big = assets_load_font(_binary_examples_mono_ttf_start, _binary_examples_mono_ttf_end, 36);
	font_very_big = assets_load_font_from_file("mono.ttf", 48);

    platform_window *window = platform_open_window(localize("window_title_main"),
                500, 500, 800, 600, 500, 500, update_render_main, 0);

    while(platform_keep_running(window)) {
        platform_handle_events();
    }

    settings_write_to_file();
    platform_destroy();

    return 0;
}