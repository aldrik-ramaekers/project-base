void _qui_close_entire_toolbar(qui_widget* el);
void _qui_close_entire_toolbar_item(qui_widget* el);
qui_widget* _qui_find_parent_of_type(qui_widget* widget, qui_widget_type type);
qui_widget* _qui_create_empty_widget(qui_widget* parent);
vec4 get_vec4_within_current_vec4(vec4 current, vec4 area);
bool _qui_mouse_interacts(qui_state* state, vec4 area);
bool _qui_mouse_interacts_peak(qui_state* state, vec4 area);
void _qui_fill_parent(qui_widget* el);
bool _qui_can_take_scroll(qui_state* state, qui_widget* el);
u8* _qui_allocate(qui_widget* wg, u32 size);

#include "qui/button.c"
#include "qui/toolbar.c"
#include "qui/toolbar_item.c"
#include "qui/toolbar_item_option.c"
#include "qui/vertical_layout.c"
#include "qui/horizontal_layout.c"
#include "qui/fixed_container.c"
#include "qui/dragbar.c"
#include "qui/itembar.c"
#include "qui/label.c"
#include "qui/separator.c"
#include "qui/dropdown.c"
#include "qui/tabcontrol.c"
#include "qui/scroll_bar.c"
#include "qui/scroll_button.c"
#include "qui/scroll.c"
#include "qui/table.c"
#include "qui/tabcontrol_panel.c"
#include "qui/dropdown_option.c"
#include "qui/size_container.c"
#include "qui/flex_container.c"
#include "qui/image_panel.c"

//////// General setup
void _qui_set_darkmode()
{
	active_ui_style.widget_text = rgb(249, 249, 249);
	active_ui_style.clear_color = rgb(38, 38, 38);
	active_ui_style.collapse_color = rgb(173, 194, 216);
	active_ui_style.scroll_background = rgb(45, 45, 45);

	active_ui_style.widget_background_static = rgb(31, 31, 31);
	active_ui_style.widget_background_interactive_idle = rgb(51, 51, 51);
	active_ui_style.widget_background_interactive_highlighted = rgb(59, 64, 71);
	active_ui_style.widget_background_interactive_hovered = rgb(61, 61, 61);
	active_ui_style.widget_background_interactive_selected_option = rgb(35, 92, 150);

	active_ui_style.widget_border_inner_idle = rgb(72, 72, 72);
	active_ui_style.widget_border_inner_highlighted = rgb(80, 85, 90);
	active_ui_style.widget_border_inner_hovered = rgb(82, 82, 82);

	active_ui_style.widget_border_outter_static = rgb(24, 24, 24);
	active_ui_style.widget_border_outter_idle = rgb(27, 27, 27);
	active_ui_style.widget_border_outter_highlighted = rgb(80, 160, 240);
	active_ui_style.widget_border_outter_hovered = rgb(27, 27, 27);

	active_ui_style.widget_resize_bar_background = active_ui_style.widget_background_static;
	active_ui_style.widget_panel_background = rgb(35, 35, 35);

	active_ui_style.widget_interactive_image_tint = rgb(255,255,255);
}

void _qui_set_lightmode()
{
	active_ui_style.widget_text = rgb(0, 0, 0);
	active_ui_style.clear_color = rgb(240, 240, 240);
	active_ui_style.collapse_color = rgb(255,255,255);
	active_ui_style.scroll_background = rgb(240, 240, 240);

	active_ui_style.widget_background_static = rgb(255,255,255);
	active_ui_style.widget_background_interactive_idle = rgb(215,215,215);
	active_ui_style.widget_background_interactive_highlighted = rgb(197, 239, 252);
	active_ui_style.widget_background_interactive_hovered = rgb(209, 244, 255);
	active_ui_style.widget_background_interactive_selected_option = rgb(209, 244, 255);

	active_ui_style.widget_border_inner_idle = rgb(240,240,240);
	active_ui_style.widget_border_inner_highlighted = rgb(166, 228, 247);
	active_ui_style.widget_border_inner_hovered = rgb(209, 244, 255);

	active_ui_style.widget_border_outter_static = rgb(160, 160, 160);
	active_ui_style.widget_border_outter_idle = rgb(163, 163, 163);
	active_ui_style.widget_border_outter_highlighted = rgb(111, 199, 227);
	active_ui_style.widget_border_outter_hovered = rgb(111, 199, 227);

	active_ui_style.widget_resize_bar_background = rgb(215, 215, 215);
	active_ui_style.widget_panel_background = rgb(255,255,255);

	active_ui_style.widget_interactive_image_tint = rgb(0,0,0);
}

void _qui_apply_theme(qui_state* state, application_theme theme)
{
	switch(theme)
	{
		case APPLICATION_THEME_LIGHT: _qui_set_lightmode(); break;
		case APPLICATION_THEME_DARK: _qui_set_darkmode(); break;
	}
	state->theme = theme;
}

void qui_set_theme(qui_widget* qui, application_theme theme, bool respect_platform_theme)
{
	qui_state* state = (qui_state*)qui->data;
	state->respect_platform_theme = respect_platform_theme;
	if (!respect_platform_theme)
	{
		_qui_apply_theme(state, theme);
	}
	else
	{
		application_theme theme = platform_get_application_theme();
		_qui_apply_theme(state, theme);	
	}

	state->window->do_draw = true;
}

void* _ui_thread_poll_platform_theme(void* args)
{
	qui_state* state = args;
	while (1)
	{
		if (state->respect_platform_theme && state->window) {
			application_theme theme = platform_get_application_theme();

			if (theme != state->theme)
			{
				_qui_apply_theme(state, theme);
				state->window->do_draw = true;
			}
		}

		thread_sleep(1000000); // 1 sec
	}

	return 0;
}

u8* _qui_allocate(qui_widget* wg, u32 size)
{
	qui_widget* main_wg = (qui_widget*)_qui_find_parent_of_type(wg, WIDGET_MAIN);
	log_assert(main_wg, "Widget is not within a QUI ui");

	qui_state* main_state = (qui_state*)main_wg->data;

	log_assert(main_state->memory_cursor + size <= main_state->memory_buffer_size, "QUI memory allocator is full");

	u8* result = main_state->memory_buffer + main_state->memory_cursor;
	main_state->memory_cursor += size;
	return result;
}

static void _qui_destroy_widget(qui_widget* widget)
{
	for (s32 i = 0; i < widget->children.length; i++) {
		qui_widget* w = *(qui_widget**)array_at(&widget->children, i);
		_qui_destroy_widget(w);
	}

	for (s32 i = 0; i < widget->special_children.length; i++) {
		qui_widget* w = *(qui_widget**)array_at(&widget->special_children, i);
		_qui_destroy_widget(w);
	}

	array_destroy(&widget->children);
	array_destroy(&widget->special_children);
}

void qui_destroy(qui_widget* qui)
{
	qui_state* state = (qui_state*)qui->data;
	_qui_destroy_widget(qui);
	mem_free(state->memory_buffer);
	mem_free(state);
	mem_free(qui);
}

qui_widget* qui_setup(u64 blocksize)
{
	qui_widget* wg = mem_alloc(sizeof(qui_widget));
	wg->children = array_create(sizeof(qui_widget*));
	wg->special_children = array_create(sizeof(qui_widget*));

	qui_state* state = mem_alloc(sizeof(qui_state));
	state->scissor_index = 0;
	memset(state->scissor_stack, 0, sizeof(state->scissor_stack));
	state->window = 0;
	state->dragging_widget = 0;

	state->memory_buffer = mem_alloc(blocksize);
	state->memory_buffer_size = blocksize;
	state->memory_cursor = 0;

	state->respect_platform_theme = true;
	state->theme = -1;
	state->font_default = assets_load_font(mono_ttf, mono_ttf+mono_ttf_len, 16);

	wg->data = (u8*)state;
	wg->type = WIDGET_MAIN;
	wg->x = 0;
	wg->y = 0;

	// Set default theme to system theme.
	application_theme theme = platform_get_application_theme();
	_qui_apply_theme(state, theme);
	
	// Start thread that checks for theme change on system.
	//thread theme_thread = thread_start(_ui_thread_poll_platform_theme, (void*)state);
	//thread_detach(&theme_thread);

	return wg;
}

//////// General qui functions
void _qui_fill_parent(qui_widget* el) {
	el->x = el->parent->x + el->margin_x;
	el->y = el->parent->y + el->margin_y;
	el->width = el->parent->width - el->margin_x*2;
	el->height = el->parent->height - el->margin_y*2;
}

bool _qui_mouse_interacts_peak(qui_state* state, vec4 area) {
	return !state->window->resizing && !state->dragging_widget && mouse_interacts_peak(area.x, area.y, area.w, area.h);
}

bool _qui_mouse_interacts(qui_state* state, vec4 area) {
	return !state->window->resizing && !state->dragging_widget && mouse_interacts(area.x, area.y, area.w, area.h);
}

bool _qui_can_take_scroll(qui_state* state, qui_widget* el)
{
	return !(state->dragging_widget != 0 && state->dragging_widget != el);
}

qui_widget* _qui_create_empty_widget(qui_widget* parent) {
	qui_widget* wg = (qui_widget*)_qui_allocate(parent, sizeof(qui_widget));
	wg->children = array_create(sizeof(qui_widget*));
	wg->special_children = array_create(sizeof(qui_widget*));
	wg->data = 0;
	wg->width = 0;
	wg->type = 0;
	wg->height = 0;
	wg->x = 0;
	wg->y = 0;
	wg->scroll_y = 0;
	wg->margin_x = 0;
	wg->margin_y = 0;
	wg->visible = true;
	wg->parent = parent;
	if (parent) {
		if (parent->type == WIDGET_FIXED_CONTAINER || parent->type == WIDGET_SIZE_CONTAINER || parent->type == WIDGET_FLEX_CONTAINER) {
			log_assert(parent->children.length == 0, "Fixed, Size and Flex containers can only contain 1 child");
		}
		array_push(&parent->children, (uint8_t*)&wg);
	}
	return wg;
}

bool _qui_is_widget_popup_type(qui_widget* el) {
	return el->type == WIDGET_TOOLBAR_ITEM_OPTION || el->type == WIDGET_DROPDOWN_OPTION;
}

vec4 get_vec4_within_current_vec4(vec4 current, vec4 area) {
	s32 x = area.x > current.x ? area.x : current.x;
	s32 y = area.y > current.y ? area.y : current.y;
	s32 w = (x + area.w) > (current.x + current.w) ? (current.x + current.w) : (x + area.w);
	s32 h = (y + area.h) > (current.y + current.h) ? (current.y + current.h) : (y + area.h);
	if (w < x) w = x;
	if (h < y) h = y;
	return (vec4){x,y,w-x,h-y};
}

void _qui_render_set_scissor(qui_state* state, qui_widget* el, bool is_special) {
	s32 border_left = 0;
	s32 border_top = 0;
	s32 border_right = 0;
	s32 border_bottom = 0;

	// Make sure we dont render on top of parent's borders
	qui_border b = BORDER_NONE;
	s32 b_size = 0;
	if (el->parent && el->parent->type == WIDGET_FLEX_CONTAINER) {
		b = ((qui_flex_container*)el->parent->data)->border;
		b_size = ((qui_flex_container*)el->parent->data)->border_size;
	}

	if (b & BORDER_LEFT) border_left += b_size;
	if (b & BORDER_TOP) border_top += b_size;
	if (b & BORDER_RIGHT) border_right += b_size;
	if (b & BORDER_BOTTOM) border_bottom += b_size;

	// Calculate scissor bounds within parent's scissor bounds
	state->scissor_stack[state->scissor_index] = state->scissor_index == 0 ? 
		(vec4){el->x, el->y, el->width, el->height} : 
		get_vec4_within_current_vec4(state->scissor_stack[state->scissor_index-1],
			(vec4){el->x + border_left, el->y + border_top,
				 el->width - border_left - border_right, el->height - border_top - border_bottom});

	// Set scissor
	if (!is_special) {
		renderer->render_set_scissor(state->window, 
			state->scissor_stack[state->scissor_index].x,
			state->scissor_stack[state->scissor_index].y,
			state->scissor_stack[state->scissor_index].w,
			state->scissor_stack[state->scissor_index].h);
	}
}

void _qui_render_widget(qui_state* state, qui_widget* el, bool draw_special) {
	bool is_special = _qui_is_widget_popup_type(el);
	if (is_special != draw_special) return;

	log_assert(state->scissor_index < 100, "Thats a very deep UI!");
	_qui_render_set_scissor(state, el, is_special);

	if (el->type == WIDGET_BUTTON) _qui_render_button(state, el);
	if (el->type == WIDGET_TOOLBAR) _qui_render_toolbar(state, el);
	if (el->type == WIDGET_TOOLBAR_ITEM) _qui_render_toolbar_item(state, el);
	if (el->type == WIDGET_TOOLBAR_ITEM_OPTION) _qui_render_toolbar_item_option(state, el);
	if (el->type == WIDGET_LABEL) _qui_render_label(state, el);
	if (el->type == WIDGET_DROPDOWN) _qui_render_dropdown(state, el);
	if (el->type == WIDGET_DROPDOWN_OPTION) _qui_render_dropdown_option(state, el);
	if (el->type == WIDGET_TABCONTROL) _qui_render_tabcontrol(state, el);
	if (el->type == WIDGET_TABCONTROL_PANEL) _qui_render_tabcontrol_panel(state, el);
	if (el->type == WIDGET_SCROLL) _qui_render_scroll(state, el);
	if (el->type == WIDGET_SCROLL_BUTTON) _qui_render_scroll_button(state, el);
	if (el->type == WIDGET_SCROLL_BAR) _qui_render_scroll_bar(state, el);
	if (el->type == WIDGET_IMAGE_PANEL) _qui_render_image_panel(state, el);

	if (el->type == WIDGET_VERTICAL_LAYOUT/* || el->type == WIDGET_MAIN*/) _qui_render_vertical_layout(state, el);
	if (el->type == WIDGET_FIXED_CONTAINER) _qui_render_fixed_container(state, el);
	if (el->type == WIDGET_SIZE_CONTAINER) _qui_render_size_container(state, el);
	if (el->type == WIDGET_FLEX_CONTAINER) _qui_render_flex_container(state, el);
	if (el->type == WIDGET_HORIZONTAL_LAYOUT) _qui_render_horizontal_layout(state, el);
	state->scissor_index++;
	for (s32 i = 0; i < el->children.length; i++) {
		qui_widget* w = *(qui_widget**)array_at(&el->children, i);
		_qui_render_widget(state, w, draw_special);
	}
	state->scissor_index--;
}

qui_widget* _qui_find_parent_of_type(qui_widget* widget, qui_widget_type type) {
	qui_widget* parent = widget;
	while (parent) {
		if (parent->type == type) return parent;
		parent = parent->parent;
	}
	return 0;
}

void _qui_update_widget(qui_state* state, qui_widget* el, bool update_special) {
	bool is_special = _qui_is_widget_popup_type(el);
	if (is_special != update_special) return;
	
	log_assert(state->scissor_index < 100, "Thats a very deep UI!");
	state->scissor_stack[state->scissor_index] = state->scissor_index == 0 ? 
		(vec4){el->x, el->y, el->width, el->height} : 
		get_vec4_within_current_vec4(state->scissor_stack[state->scissor_index-1], (vec4){el->x, el->y, el->width, el->height});

	// Update elements that dont take input.... maybe rename functions to something like _qui_resize..
	if (el->type == WIDGET_SCROLL) _qui_update_scroll(state, el);
	if (el->type == WIDGET_VERTICAL_LAYOUT/* || el->type == WIDGET_MAIN*/) _qui_update_vertical_layout(state, el);
	if (el->type == WIDGET_HORIZONTAL_LAYOUT) _qui_update_horizontal_layout(state, el);

	state->scissor_index++;
	for (s32 i = 0; i < el->children.length; i++) {
		qui_widget* w = *(qui_widget**)array_at(&el->children, i);
		_qui_update_widget(state, w, update_special);
	}
	state->scissor_index--;

	if (el->type == WIDGET_BUTTON) _qui_update_button(state, el);
	if (el->type == WIDGET_TOOLBAR) _qui_update_toolbar(state, el);
	if (el->type == WIDGET_TOOLBAR_ITEM) _qui_update_toolbar_item(state, el);
	if (el->type == WIDGET_TOOLBAR_ITEM_OPTION) _qui_update_toolbar_item_option(state, el);
	if (el->type == WIDGET_LABEL) _qui_update_label(state, el);
	if (el->type == WIDGET_DROPDOWN) _qui_update_dropdown(state, el);
	if (el->type == WIDGET_DROPDOWN_OPTION) _qui_update_dropdown_option(state, el);
	if (el->type == WIDGET_TABCONTROL) _qui_update_tabcontrol(state, el);
	if (el->type == WIDGET_TABCONTROL_PANEL) _qui_update_tabcontrol_panel(state, el);
	if (el->type == WIDGET_SCROLL_BUTTON) _qui_update_scroll_button(state, el);
	if (el->type == WIDGET_SCROLL_BAR) _qui_update_scroll_bar(state, el);
	if (el->type == WIDGET_IMAGE_PANEL) _qui_update_image_panel(state, el);

	if (el->type == WIDGET_SIZE_CONTAINER) _qui_update_size_container(state, el);
	//if (el->type == WIDGET_FIXED_CONTAINER) _qui_update_fixed_container(el);
	//if (el->type == WIDGET_FLEX_CONTAINER) _qui_update_flex_container(el);
}

void qui_render(platform_window* window, qui_widget* el) {
	log_assert(el->type == WIDGET_MAIN, "qui_render must be called with the main widget");

	qui_state* state = (qui_state*)el->data;

	renderer->render_clear(window, active_ui_style.clear_color);
	state->scissor_stack[0] = (vec4){0,0,0,0};
	state->scissor_index = 0;
	state->window = window;
	renderer->render_reset_scissor();

	for (s32 i = 0; i < el->children.length; i++) {
		qui_widget* w = *(qui_widget**)array_at(&el->children, i);
		_qui_render_widget(state, w, false);
	}

	state->scissor_stack[0] = (vec4){0,0,0,0};
	state->scissor_index = 0;
	renderer->render_reset_scissor();

	for (s32 i = 0; i < el->special_children.length; i++) {
		qui_widget* w = *(qui_widget**)array_at(&el->special_children, i);
		_qui_render_widget(state, w, true);
	}
}

void qui_update(platform_window* window, qui_widget* el) {
	log_assert(el->type == WIDGET_MAIN, "qui_update must be called with the main widget");

	qui_state* state = (qui_state*)el->data;

	state->window = window;
	el->width = window->width;
	el->height = window->height;

	// Update popup types first.
	for (s32 i = 0; i < el->special_children.length; i++) {
		qui_widget* w = *(qui_widget**)array_at(&el->special_children, i);
		_qui_update_widget(state, w, true);
	}

	// If mouse press was not handled by popup types, they should be closed.
	if (is_left_clicked_peak()) {
		for (s32 i = 0; i < el->special_children.length; i++) {
			qui_widget* w = *(qui_widget**)array_at(&el->special_children, i);
			if (w->type == WIDGET_TOOLBAR_ITEM_OPTION) _qui_close_entire_toolbar(w);
			if (w->type == WIDGET_DROPDOWN_OPTION) _qui_close_dropdown(w);
		}
	}

	// Update everything else.
	for (s32 i = 0; i < el->children.length; i++) {
		qui_widget* w = *(qui_widget**)array_at(&el->children, i);
		//for (int x = 0; x < 5; x++)
		_qui_update_widget(state, w, false);
	}
}