/* 
*  BSD 2-Clause “Simplified” License
*  Copyright (c) 2019, Aldrik Ramaekers, aldrik.ramaekers@protonmail.com
*  All rights reserved.
*/

#include "config.h"
#include "project_base.h"

// TODO(Aldrik): option to disable menu item
// TODO(Aldrik): move the delete button for term to edit panel on the topright and put a exclamation mark at the old spot to indicate a missing translation
// TODO(Aldrik): language name validation
// TODO(Aldrik): language mo file name preview when entering name
// TODO(Aldrik): option to mark languages with colors
// TODO(Aldrik): change save icon

s32 global_language_id = 1;
char project_path[MAX_INPUT_LENGTH];

#define MAX_LANGUAGE_COUNT 100

typedef struct t_language
{
	char *name;
	s32 id;
} language;

typedef struct t_translation
{
	s32 language_id;
	char *value;
} translation;

typedef struct t_term
{
	char *name;
	array translations;
} term;

typedef struct t_translation_project
{
	array languages;
	array terms;
	int selected_term_index;
} translation_project;

translation_project *current_project = 0;

#include "save.h"
#include "save.c"

scroll_state term_scroll;
scroll_state lang_scroll;
scroll_state trans_scroll;
button_state btn_new_project;
button_state btn_new_language;
button_state btn_summary;
button_state btn_set_term_name;
dropdown_state dd_available_countries;
textbox_state tb_filter;
textbox_state tb_new_term;
textbox_state tb_new_language;
textbox_state tb_translation_list[MAX_LANGUAGE_COUNT];

image *set_img;
image *add_img;
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
	add_img = assets_load_image(_binary____data_imgs_add_png_start,
								_binary____data_imgs_add_png_end, false);
	set_img = assets_load_image(_binary____data_imgs_set_png_start,
								_binary____data_imgs_set_png_end, false);
	
	font_medium = assets_load_font(_binary____data_fonts_mono_ttf_start,
								   _binary____data_fonts_mono_ttf_end, 18);
	font_small = assets_load_font(_binary____data_fonts_mono_ttf_start,
								  _binary____data_fonts_mono_ttf_end, 15);
	font_mini = assets_load_font(_binary____data_fonts_mono_ttf_start,
								 _binary____data_fonts_mono_ttf_end, 12);
}

bool term_has_missing_translations(term *t)
{
	for (s32 i = 0; i < t->translations.length; i++)
	{
		translation *tr = array_at(&t->translations, i);
		if (string_equals(tr->value, "")) return true;
	}
	
	return false;
}

s32 get_translated_count_for_language(s32 language_id)
{
	s32 count = 0;
	for (s32 i = 0; i < current_project->terms.length; i++)
	{
		term *t = array_at(&current_project->terms, i);
		
		for (s32 x = 0; x < t->translations.length; x++)
		{
			translation *tr = array_at(&t->translations, x);
			
			if (tr->language_id == language_id && tr->value) 
			{
				count++;
				break;
			}
		}
	}
	
	return count;
}

bool term_name_is_available(char *name)
{
	for (s32 i = 0; i < current_project->terms.length; i++)
	{
		term *tr = array_at(&current_project->terms, i);
		
		if (string_equals(tr->name, name))
		{
			return false;
		}
	}
	
	return true;
}

language* get_language_by_id(s32 language_id)
{
	for (s32 i = 0; i < current_project->languages.length; i++)
	{
		language *l = array_at(&current_project->languages, i);
		if (l->id == language_id)
		{
			return l;
		}
	}
	return 0;
}

s32 add_language_to_project(char *buffer)
{
	if (string_equals(buffer, ""))
	{
		// TODO(Aldrik): localize
		platform_show_message(main_window, "Language name cannot be empty", "Invalid info");
		return -1;
	}
	
	char *val = mem_alloc(MAX_INPUT_LENGTH);
	string_copyn(val, buffer, MAX_INPUT_LENGTH);
	
	language l;
	l.name = val;
	l.id = global_language_id++;
	
	s32 index = array_push(&current_project->languages, &l);
	
	for (s32 x = 0; x < current_project->terms.length; x++)
	{
		term *t = array_at(&current_project->terms, x);
		
		translation new_t;
		new_t.value = mem_alloc(MAX_INPUT_LENGTH);
		string_copyn(new_t.value, "", MAX_INPUT_LENGTH);
		new_t.language_id = l.id;
		
		array_push(&t->translations, &new_t);
	}
	
	return l.id;
}

void set_term_name(s32 index, char *name)
{
	if (strlen(name) > 0)
	{
		term *t = array_at(&current_project->terms, index);
		string_copyn(t->name, name, MAX_TERM_NAME_LENGTH);
	}
	else
	{
		// TODO(Aldrik): translate
		platform_show_message(main_window, "Term name cannot be empty", "Invalid input");
	}
}

void remove_term_from_project(s32 index)
{
	term *t = array_at(&current_project->terms, index);
	mem_free(t->name);
	for (s32 i = 0; i < t->translations.length; i++)
	{
		translation *tr = array_at(&t->translations, i);
		mem_free(tr->value);
	}
	array_destroy(&t->translations);
	array_remove_at(&current_project->terms, index);
}

void remove_language_from_project(s32 language_id)
{
	for (s32 i = 0; i < current_project->languages.length; i++)
	{
		language *l = array_at(&current_project->languages, i);
		if (l->id == language_id)
		{
			array_remove_at(&current_project->languages, i);
			break;
		}
	}
	
	for (s32 x = 0; x < current_project->terms.length; x++)
	{
		term *tr = array_at(&current_project->terms, x);
		
		for (s32 i = 0; i < tr->translations.length; i++)
		{
			translation *t = array_at(&tr->translations, i);
			
			if (t->language_id == language_id)
			{
				mem_free(t->value);
				array_remove_at(&tr->translations, i);
				break;
			}
		}
	}
}

void select_term(s32 index)
{
	current_project->selected_term_index = index;
	term *t = array_at(&current_project->terms, index);
	ui_set_textbox_text(&tb_new_term, t->name);
	
	for (s32 i = 0; i < current_project->languages.length; i++)
	{
		translation *tr = array_at(&t->translations, i);
		textbox_state *tb = &tb_translation_list[i];
		tb->state = false;
		
		if (tr->value)
		{
			ui_set_textbox_text(tb, tr->value);
		}
		else
		{
			ui_set_textbox_text(tb, "");
		}
	}
}

s32 add_term_to_project()
{
	tb_filter.state = 0;
	ui_set_textbox_text(&tb_filter, "");
	
	term t;
	t.name = mem_alloc(MAX_TERM_NAME_LENGTH);
	
	s32 count = 0;
	do
	{
		char buffer[MAX_TERM_NAME_LENGTH];
		sprintf(buffer, "term_%d", count);
		string_copyn(t.name, buffer, MAX_TERM_NAME_LENGTH);
		count++;
	}
	while(!term_name_is_available(t.name));
	
	t.translations = array_create(sizeof(translation));
	array_reserve(&t.translations, current_project->languages.length);
	
	for (s32 i = 0; i < current_project->languages.length; i++)
	{
		language *l = array_at(&current_project->languages, i);
		
		translation tr;
		tr.value = mem_alloc(MAX_INPUT_LENGTH);
		string_copyn(tr.value, "", MAX_INPUT_LENGTH);
		tr.language_id = l->id;
		array_push(&t.translations, &tr);
	}
	
	return array_push(&current_project->terms, &t);
}

void save_term_changes()
{
	set_term_name(current_project->selected_term_index, tb_new_term.buffer);
	
	term *t = array_at(&current_project->terms, current_project->selected_term_index);
	
	for (s32 i = 0; i < current_project->languages.length; i++)
	{
		translation *tr = array_at(&t->translations, i);
		textbox_state *tb = &tb_translation_list[i];
		
		string_copyn(tr->value, tb->buffer, MAX_INPUT_LENGTH);
	}
}

void start_new_project()
{
	global_language_id = 1;
	current_project = mem_alloc(sizeof(translation_project));
	
	current_project->terms = array_create(sizeof(term));
	array_reserve(&current_project->terms, 100);
	current_project->terms.reserve_jump = 100;
	
	current_project->languages = array_create(sizeof(language));
	array_reserve(&current_project->languages, 100);
	current_project->languages.reserve_jump = 100;
	
	current_project->selected_term_index = -1;
}

void load_config(settings_config *config)
{
	char *path = settings_config_get_string(config, "ACTIVE_PROJECT");
	char *locale_id = settings_config_get_string(config, "LOCALE");
	
	if (path)
	{
		string_copyn(project_path, path, MAX_INPUT_LENGTH);
		
		if (platform_directory_exists(project_path))
		{
			load_project_from_folder(project_path);
		}
		else
		{
			string_copyn(project_path, "", MAX_INPUT_LENGTH);
		}
	}
	
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
	if (window_w < 800 || window_h < 600)
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
	term_scroll = ui_create_scroll(1);
	lang_scroll = ui_create_scroll(1);
	trans_scroll = ui_create_scroll(1);
	btn_summary = ui_create_button();
	btn_set_term_name = ui_create_button();
	btn_new_project = ui_create_button();
	btn_new_language = ui_create_button();
	tb_filter = ui_create_textbox(MAX_INPUT_LENGTH);
	tb_new_term = ui_create_textbox(MAX_TERM_NAME_LENGTH);
	tb_new_language = ui_create_textbox(MAX_INPUT_LENGTH);
	
	for (s32 i = 0; i < MAX_LANGUAGE_COUNT; i++)
		tb_translation_list[i] = ui_create_textbox(MAX_INPUT_LENGTH);
	
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
						load_project();
					}
					// TODO(Aldrik): translate
					if (ui_push_menu_item("Save Project", "Ctrl + S")) 
					{ 
						if (string_equals(project_path, ""))
							save_project();
						else
							save_project_to_folder(project_path);
					}
					// TODO(Aldrik): translate
					if (ui_push_menu_item("Save Project As", "Ctrl + E"))  
					{ 
						save_project();
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
					if (ui_push_button_image(&btn_summary, "", list_img))
					{
						current_project->selected_term_index = -1;
					}
					
					// TODO(Aldrik): translate
					ui_push_textf_width(font_medium, "Terms", global_ui_context.layout.width-150);
					
					if (ui_push_button_image(&btn_summary, "", add_img))
					{
						select_term(add_term_to_project());
					}
					
					//ui_push_button_image(&btn_summary, "", delete_img);
					
				}
				ui_block_end();
				
				ui_block_begin(LAYOUT_HORIZONTAL);
				{
					// TODO(Aldrik): translate
					TEXTBOX_WIDTH = 280;
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
						
						if (!strlen(tb_filter.buffer) || string_contains(t->name, tb_filter.buffer))
						{
							bool removed = false;
							if (ui_push_button_image(&btn_summary, "", delete_img))
							{
								removed = true;
							}
							
							//ui_push_image(exclaim_img, 14, 14, 1, rgb(255,255,255));
							
							if (i == current_project->selected_term_index)
							{
								ui_push_rect(10, global_ui_context.style.textbox_active_border);
							}
							else if (term_has_missing_translations(t))
							{
								ui_push_rect(10, MISSING_TRANSLATION_COLOR);
							}
							else
							{
								ui_push_rect(10, global_ui_context.style.background);
							}
							
							if (ui_push_text_width(t->name, global_ui_context.layout.width-100, true))
							{
								select_term(i);
							}
							
							ui_block_end();
							
							if (removed)
							{
								remove_term_from_project(i);
								--i;
								
								if (current_project->selected_term_index >= 0)
									current_project->selected_term_index--;
							}
						}
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
				if (keyboard_is_key_down(&keyboard, KEY_LEFT_CONTROL) && keyboard_is_key_pressed(&keyboard, KEY_S))
				{
					save_term_changes();
				}
				
				term *t = array_at(&current_project->terms, 
								   current_project->selected_term_index);
				
				ui_block_begin(LAYOUT_HORIZONTAL);
				{
					// editor
					
					if (string_equals(tb_new_term.buffer, t->name))
						ui_push_rect(10, global_ui_context.style.background);
					else
						ui_push_rect(10, UNSAVED_CHANGES_COLOR);
					
					// TODO(Aldrik): localize
					ui_push_textbox(&tb_new_term, "Term name");
					
					if (ui_push_button_image(&btn_set_term_name, "", set_img))
					{
						save_term_changes();
					}
				}
				ui_block_end();
				
				global_ui_context.layout.offset_x = 310;
				ui_push_separator();
				
				trans_scroll.height = main_window->height-global_ui_context.layout.offset_y;
				
				ui_scroll_begin(&trans_scroll);
				{
					if (!current_project->languages.length)
					{
						// TODO(Aldrik): localize
						ui_push_text("No languages added to project yet.");
					}
					else
					{
						for (s32 i = 0; i < t->translations.length; i++)
						{
							translation *tr = array_at(&t->translations, i);
							
							TEXTBOX_WIDTH = global_ui_context.layout.width - 130;
							
							if (!strlen(tb_translation_list[i].buffer))
							{
								ui_push_rect(10, MISSING_TRANSLATION_COLOR);
							}
							else if (string_equals(tb_translation_list[i].buffer, 
												   tr->value))
							{
								ui_push_rect(10, global_ui_context.style.background);
							}
							else
							{
								ui_push_rect(10, UNSAVED_CHANGES_COLOR);
							}
							
							ui_push_textbox(&tb_translation_list[i], "");
							
							language *l = get_language_by_id(tr->language_id);
							ui_push_text(l->name);
							
							global_ui_context.layout.offset_y += TEXTBOX_HEIGHT + WIDGET_PADDING;
							global_ui_context.layout.offset_x = 310;
						}
					}
				}
				ui_scroll_end();
			}
			else if (current_project)
			{
				// overview
				ui_block_begin(LAYOUT_HORIZONTAL);
				{
					// TODO(Aldrik): translate
					ui_push_textf_width(font_medium, "Overview", 100);
					
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
					// TODO(Aldrik): translate
					bool selected = tb_new_language.state;
					ui_push_textbox(&tb_new_language, "Add language");
					
					if (keyboard_is_key_pressed(&keyboard, KEY_ENTER) && selected)
					{
						add_language_to_project(tb_new_language.buffer);
						ui_set_textbox_text(&tb_new_language, "");
						keyboard_set_input_text(&keyboard, "");
						tb_new_language.state = true;
					}
					
					// TODO(Aldrik): translate
					if (ui_push_button(&btn_new_language, "Add"))
					{
						add_language_to_project(tb_new_language.buffer);
						ui_set_textbox_text(&tb_new_language, "");
						keyboard_set_input_text(&keyboard, "");
						tb_new_language.state = true;
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
						
						language *l = array_at(&current_project->languages, i);
						ui_push_text_width(l->name, global_ui_context.layout.width-200, false);
						
						color c = global_ui_context.style.foreground;
						global_ui_context.style.foreground = rgb(110,110,110);
						
						char stats[50];
						sprintf(stats, "%d/%d translated", get_translated_count_for_language(l->id), current_project->terms.length);
						ui_push_text(stats);
						
						global_ui_context.style.foreground = c;
						
						if (pressed)
						{
							remove_language_from_project(l->id);
							i--;
						}
						
						ui_block_end();
					}
				}
				ui_scroll_end();
			}
			else
			{
				render_text(font_medium, 330, 50, "Create a new project", global_ui_context.style.foreground);
				render_text_cutoff(font_small, 330, 70,
								   "There is no project loaded yet, you can start a\nnew translation project on the left.", global_ui_context.style.foreground, 400);
				
				render_rectangle(330, 200, 10, 25, UNSAVED_CHANGES_COLOR);
				render_text(font_small, 350, 208, "Unsaved changes", global_ui_context.style.foreground);
				
				render_rectangle(330, 240, 10, 25, MISSING_TRANSLATION_COLOR);
				render_text(font_small, 350, 248, "Missing translation", global_ui_context.style.foreground);
			}
			
			if (keyboard_is_key_down(&keyboard, KEY_LEFT_CONTROL) && keyboard_is_key_pressed(&keyboard, KEY_O))
			{
				load_project();
			}
			if (keyboard_is_key_down(&keyboard, KEY_LEFT_CONTROL) && keyboard_is_key_pressed(&keyboard, KEY_S))
			{
				if (string_equals(project_path, ""))
					save_project();
				else
					save_project_to_folder(project_path);
			}
			if (keyboard_is_key_down(&keyboard, KEY_LEFT_CONTROL) && keyboard_is_key_pressed(&keyboard, KEY_E))
			{
				save_project();
			}
			if (keyboard_is_key_down(&keyboard, KEY_LEFT_CONTROL) && keyboard_is_key_pressed(&keyboard, KEY_Q))
			{
				window.is_open = false; 
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
	if (!string_equals(project_path, ""))
		settings_config_set_string(&config, "ACTIVE_PROJECT", project_path);
	
	vec2 win_size = platform_get_window_size(&window);
	settings_config_set_number(&config, "WINDOW_WIDTH", win_size.x);
	settings_config_set_number(&config, "WINDOW_HEIGHT", win_size.y);
	
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
	assets_destroy_image(add_img);
	
	assets_destroy_font(font_small);
	assets_destroy_font(font_mini);
	
	keyboard_input_destroy(&keyboard);
	platform_destroy_window(&window);
	
	platform_destroy();
	
	return 0;
}
#endif