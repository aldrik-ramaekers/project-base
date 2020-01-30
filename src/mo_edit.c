/* 
*  BSD 2-Clause “Simplified” License
*  Copyright (c) 2019, Aldrik Ramaekers, aldrik.ramaekers@protonmail.com
*  All rights reserved.
*/

#include "config.h"
#include "project_base.h"

#include "languages.h"

// TODO(Aldrik): option to disable menu item

typedef struct t_translation
{
	s32 country_index;
	char *value;
} translation;

typedef struct t_term
{
	char *name;
	translation translations[COUNTRY_CODE_COUNT];
} term;

typedef struct t_translation_project
{
	array languages;
	array terms;
	int selected_term_index;
} translation_project;

translation_project *current_project = 0;

scroll_state term_scroll;
scroll_state lang_scroll;
button_state btn_new_project;
button_state btn_new_language;
button_state btn_summary;
dropdown_state dd_available_countries;
textbox_state tb_filter;

image *list_img;
image *exclaim_img;
image *delete_img;
image *logo_small_img;

font *font_medium;
font *font_small;
font *font_mini;
s32 scroll_y = 0;

#include "settings.h"
#include "settings.c"

static void load_assets()
{
	list_img = assets_load_image(_binary____data_imgs_list_png_start, 
								 _binary____data_imgs_list_png_end, false);
	exclaim_img = assets_load_image(_binary____data_imgs_exclaim_png_start, 
									_binary____data_imgs_exclaim_png_end, false);
	logo_small_img = assets_load_image(_binary____data_imgs_logo_64_png_start,
									   _binary____data_imgs_logo_64_png_end, true);
	delete_img = assets_load_image(_binary____data_imgs_delete_png_start,
								   _binary____data_imgs_delete_png_end, false);
	
	font_medium = assets_load_font(_binary____data_fonts_mono_ttf_start,
								   _binary____data_fonts_mono_ttf_end, 18);
	font_small = assets_load_font(_binary____data_fonts_mono_ttf_start,
								  _binary____data_fonts_mono_ttf_end, 15);
	font_mini = assets_load_font(_binary____data_fonts_mono_ttf_start,
								 _binary____data_fonts_mono_ttf_end, 12);
}

s32 get_available_country_index()
{
	s32 found_index = -1;
	for (s32 x = 0; x < COUNTRY_CODE_COUNT; x++)
	{
		bool found = false;
		for (s32 i = 0; i < current_project->languages.length; i++)
		{
			s32 ind = *(s32*)array_at(&current_project->languages, i);
			if (ind == x) found = true;
		}
		
		if (!found) found_index = x;
	}
	
	return found_index;
}

bool country_has_been_added_to_project(s32 index)
{
	for (s32 i = 0; i < current_project->languages.length; i++)
	{
		s32 ind = *(s32*)array_at(&current_project->languages, i);
		
		if (index == ind) return true;
	}
	
	return false;
}

s32 get_translated_count_for_language(s32 index)
{
	s32 count = 0;
	for (s32 i = 0; i < current_project->terms.length; i++)
	{
		term *t = array_at(&current_project->terms, i);
		
		for (s32 x = 0; x < COUNTRY_CODE_COUNT; x++)
		{
			translation *tr = &t->translations[x];
			if (tr->country_index == i)
			{
				count++;
			}
		}
	}
	
	return count;
}

void start_new_project()
{
	current_project = mem_alloc(sizeof(translation_project));
	
	current_project->terms = array_create(sizeof(term));
	array_reserve(&current_project->terms, 100);
	current_project->terms.reserve_jump = 100;
	
	current_project->languages = array_create(sizeof(s32));
	array_reserve(&current_project->languages, 100);
	current_project->languages.reserve_jump = 100;
	
	for (s32 i = 0; i < 3; i++)
	{
		term t;
		t.name = mem_alloc(10);
		string_copyn(t.name, "meme_1", 10);
		
		for (s32 x = 0; x < COUNTRY_CODE_COUNT; x++)
		{
			translation tr;
			tr.value = 0;
			tr.country_index = -1;
			t.translations[x] = tr;
		}
		
		array_push(&current_project->terms, &t);
	}
	
	current_project->selected_term_index = -1;
}

void load_config(settings_config *config)
{
	char *path = settings_config_get_string(config, "ACTIVE_PROJECT");
	char *locale_id = settings_config_get_string(config, "LOCALE");
	
	if (locale_id)
		set_locale(locale_id);
	else
		set_locale("en");
}

#if defined(OS_LINUX) || defined(OS_WIN)
int main(int argc, char **argv)
{
	platform_init(argc, argv);
	
	bool is_command_line_run = (argc > 1);
	if (is_command_line_run)
	{
		handle_command_line_arguments(argc, argv);
		return 0;
	}
	
	char config_path_buffer[PATH_MAX];
	get_config_save_location(config_path_buffer);
	
	// load config
	settings_config config = settings_config_load_from_file(config_path_buffer);
	
	s32 window_w = settings_config_get_number(&config, "WINDOW_WIDTH");
	s32 window_h = settings_config_get_number(&config, "WINDOW_HEIGHT");
	if (window_w <= 800 || window_h <= 600)
	{
		window_w = 800;
		window_h = 600;
	}
	
	platform_window window = platform_open_window("mo-edit", window_w, window_h, 0, 0, 800, 600);
	main_window = &window;
	
	settings_page_create();
	
	load_available_localizations();
	set_locale("en");
	
	load_assets();
	
	keyboard_input keyboard = keyboard_input_create();
	mouse_input mouse = mouse_input_create();
	
	camera camera;
	camera.x = 0;
	camera.y = 0;
	camera.rotation = 0;
	
	ui_create(&window, &keyboard, &mouse, &camera, font_small);
	dd_available_countries = ui_create_dropdown();
	term_scroll = ui_create_scroll(1);
	lang_scroll = ui_create_scroll(1);
	btn_summary = ui_create_button();
	btn_new_project = ui_create_button();
	btn_new_language = ui_create_button();
	tb_filter = ui_create_textbox(MAX_INPUT_LENGTH);
	
	// asset worker
	thread asset_queue_worker1 = thread_start(assets_queue_worker, NULL);
	thread asset_queue_worker2 = thread_start(assets_queue_worker, NULL);
	thread_detach(&asset_queue_worker1);
	thread_detach(&asset_queue_worker2);
	
	load_config(&config);
	
	while(window.is_open) {
        u64 last_stamp = platform_get_time(TIME_FULL, TIME_US);
		platform_handle_events(&window, &mouse, &keyboard);
		platform_set_cursor(&window, CURSOR_DEFAULT);
		
		settings_page_update_render();
		
		platform_window_make_current(&window);
		
		static bool icon_loaded = false;
		if (!icon_loaded && logo_small_img->loaded)
		{
			icon_loaded = true;
			platform_set_icon(&window, logo_small_img);
		}
		
		if (global_asset_collection.queue.queue.length == 0 && !global_asset_collection.done_loading_assets)
		{
			global_asset_collection.done_loading_assets = true;
		}
		
		global_ui_context.layout.active_window = &window;
		global_ui_context.keyboard = &keyboard;
		global_ui_context.mouse = &mouse;
		
		render_clear();
		camera_apply_transformations(&window, &camera);
		
		global_ui_context.layout.width = global_ui_context.layout.active_window->width;
		// begin ui
		
		ui_begin(1);
		{
			render_rectangle(0, 0, main_window->width, main_window->height, global_ui_context.style.background);
			
			ui_begin_menu_bar();
			{
				if (ui_push_menu(localize("file")))
				{
					// TODO(Aldrik): translate
					if (ui_push_menu_item("Load Project", "Ctrl + O")) 
					{ 
					}
					// TODO(Aldrik): translate
					if (ui_push_menu_item("Save Project", "Ctrl + S")) 
					{ 
					}
					// TODO(Aldrik): translate
					if (ui_push_menu_item("Save Project As", "Ctrl + E"))  
					{ 
					}
					// TODO(Aldrik): translate
					if (ui_push_menu_item("Export MO files", "Ctrl + X"))  
					{ 
					}
					ui_push_menu_item_separator();
					if (ui_push_menu_item(localize("quit"), "Ctrl + Q")) 
					{ 
						window.is_open = false; 
					}
				}
			}
			ui_end_menu_bar();
			
			
			// TODO(Aldrik): make this a setting, resizable panel
			global_ui_context.layout.width = 300;
			ui_push_vertical_dragbar();
			
			if (current_project)
			{
				ui_block_begin(LAYOUT_HORIZONTAL);
				{
					ui_push_button_image(&btn_summary, "", list_img);
					// TODO(Aldrik): translate
					ui_push_textf_width(font_medium, "Terms", global_ui_context.layout.width-150);
					
					ui_push_button_image(&btn_summary, "", delete_img);
					ui_push_button_image(&btn_summary, "", delete_img);
					
				}
				ui_block_end();
				
				ui_block_begin(LAYOUT_HORIZONTAL);
				{
					// TODO(Aldrik): translate
					ui_push_textbox(&tb_filter, "Filter terms..");
				}
				ui_block_end();
				
				ui_push_separator();
				
				term_scroll.height = main_window->height-global_ui_context.layout.offset_y;
				ui_scroll_begin(&term_scroll);
				{
					for (s32 i = 0; i < current_project->terms.length; i++)
					{
						term *t = array_at(&current_project->terms, i);
						
						ui_push_button_image(&btn_summary, "", delete_img);
						//ui_push_image(exclaim_img, 14, 14, 1, rgb(255,255,255));
						ui_push_text_width(t->name, global_ui_context.layout.width-100);
						
						ui_block_end();
					}
				}
				ui_scroll_end();
			}
			else
			{
				// TODO(Aldrik): translate
				if (ui_push_button(&btn_new_project, "Create new project"))
				{
					start_new_project();
				}
			}
			
			global_ui_context.layout.width = main_window->width - 310;
			
			global_ui_context.layout.offset_x = 310;
			global_ui_context.layout.offset_y = MENU_BAR_HEIGHT + WIDGET_PADDING;
			
			if (current_project && current_project->selected_term_index >= 0)
			{
				
			}
			else if (current_project)
			{
				// overview
				ui_block_begin(LAYOUT_HORIZONTAL);
				{
					// TODO(Aldrik): translate
					ui_push_textf_width(font_medium, "Overview", 200);
					
					char info_text[60];
					sprintf(info_text, "%d terms, %d languages", current_project->terms.length, current_project->languages.length);
					
					color c = global_ui_context.style.foreground;
					global_ui_context.style.foreground = rgb(110,110,110);
					ui_push_textf(font_small, info_text);
					global_ui_context.style.foreground = c;
				}
				ui_block_end();
				
				ui_push_separator();
				
				ui_block_begin(LAYOUT_HORIZONTAL);
				{
					s32 av_index = get_available_country_index();
					
					if (dd_available_countries.selected_index == -1 && av_index >= 0)
						dd_available_countries.selected_index = av_index;
					
					if (dd_available_countries.selected_index >= 0)
					{
						if (ui_push_dropdown(&dd_available_countries, 
											 global_langues[dd_available_countries.selected_index].fullname))
						{
							for (s32 i = 0; i < COUNTRY_CODE_COUNT; i++)
							{
								if (!country_has_been_added_to_project(i))
								{
									if (ui_push_dropdown_item(0, global_langues[i].fullname, i))
									{
										
									}
								}
							}
						}
						
						// TODO(Aldrik): translate
						if (ui_push_button(&btn_new_language, "Add"))
						{
							array_push(&current_project->languages,&dd_available_countries.selected_index);
							
							dd_available_countries.selected_index = -1;
						}
					}
				}
				ui_block_end();
				
				if (dd_available_countries.selected_index >= 0)
					ui_push_separator();
				
				// languages
				lang_scroll.height = main_window->height-global_ui_context.layout.offset_y;
				ui_scroll_begin(&lang_scroll);
				{
					for (s32 i = 0; i < current_project->languages.length; i++)
					{
						button_state btn_remove = ui_create_button();
						
						bool pressed = false;
						if (ui_push_button_image(&btn_remove, "", delete_img))
						{
							pressed = true;
						}
						
						s32 index = *(s32*)array_at(&current_project->languages, i);
						ui_push_text_width(global_langues[index].fullname, global_ui_context.layout.width-200);
						
						color c = global_ui_context.style.foreground;
						global_ui_context.style.foreground = rgb(110,110,110);
						
						char stats[50];
						sprintf(stats, "%d/%d translated", get_translated_count_for_language(index), current_project->terms.length);
						ui_push_text(stats);
						
						global_ui_context.style.foreground = c;
						
						if (pressed)
						{
							array_remove_at(&current_project->languages, i);
							i--;
						}
						
						ui_block_end();
					}
				}
				ui_scroll_end();
			}
			else
			{
				// show no project loaded message/image
			}
		}
		ui_end();
		// end ui
		
		assets_do_post_process();
		platform_window_swap_buffers(&window);
		
		u64 current_stamp = platform_get_time(TIME_FULL, TIME_US);
		u64 diff = current_stamp - last_stamp;
		float diff_ms = diff / 1000.0f;
		last_stamp = current_stamp;
		
		if (diff_ms < TARGET_FRAMERATE)
		{
			double time_to_wait = (TARGET_FRAMERATE) - diff_ms;
			thread_sleep(time_to_wait*1000);
		}
    }
	
	settings_page_hide_without_save();
	
	// write config file
#if 0
	settings_config_set_string(&config, "SEARCH_DIRECTORY", textbox_path.buffer);
	settings_config_set_number(&config, "SEARCH_DIRECTORIES", checkbox_recursive.state);
	settings_config_set_string(&config, "SEARCH_TEXT", textbox_search_text.buffer);
	settings_config_set_string(&config, "FILE_FILTER", textbox_file_filter.buffer);
	settings_config_set_number(&config, "MAX_THEAD_COUNT", global_settings_page.max_thread_count);
	settings_config_set_number(&config, "MAX_FILE_SIZE", global_settings_page.max_file_size);
	
	vec2 win_size = platform_get_window_size(&window);
	settings_config_set_number(&config, "WINDOW_WIDTH", win_size.x);
	settings_config_set_number(&config, "WINDOW_HEIGHT", win_size.y);
	
	settings_config_set_number(&config, "STYLE", global_ui_context.style.id);
	settings_config_set_number(&config, "DOUBLE_CLICK_ACTION", global_settings_page.selected_double_click_selection_option);
	
	if (global_localization.active_localization != 0)
	{
		char *current_locale_id = locale_get_id();
		if (current_locale_id)
		{
			settings_config_set_string(&config, "LOCALE", current_locale_id);
		}
	}
	
	settings_config_write_to_file(&config, config_path_buffer);
	settings_config_destroy(&config);
#endif
	
	settings_page_destroy();
	
	destroy_available_localizations();
	
#if 0
	// cleanup ui
	ui_destroy_textbox(&textbox_path);
	ui_destroy_textbox(&textbox_search_text);
	ui_destroy_textbox(&textbox_file_filter);
#endif
	ui_destroy();
	
	// delete assets
	assets_destroy_image(list_img);
	assets_destroy_image(logo_small_img);
	assets_destroy_image(delete_img);
	
	assets_destroy_font(font_small);
	assets_destroy_font(font_mini);
	
	keyboard_input_destroy(&keyboard);
	platform_destroy_window(&window);
	
	platform_destroy();
	
	return 0;
}
#endif