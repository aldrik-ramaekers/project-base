void _qui_close_entire_toolbar(qui_widget* el);
void _qui_close_entire_toolbar_item(qui_widget* el);
qui_widget* _qui_find_parent_of_type(qui_widget* widget, qui_widget_type type);
qui_widget* _qui_create_empty_widget(qui_widget* parent);
vec4 get_vec4_within_current_vec4(vec4 current, vec4 area);
bool _qui_mouse_interacts(qui_state* state, vec4 area);
bool _qui_mouse_interacts_peak(qui_state* state, vec4 area);
void _qui_fill_parent(qui_widget* el);
bool _qui_can_take_scroll(qui_state* state, qui_widget* el);

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
qui_widget* qui_setup()
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

	qui_widget* wg = mem_alloc(sizeof(qui_widget));
	wg->children = array_create(sizeof(qui_widget*));
	wg->special_children = array_create(sizeof(qui_widget*));
	qui_state* state = mem_alloc(sizeof(qui_state));
	state->scissor_index = 0;
	memset(state->scissor_stack, 0, sizeof(state->scissor_stack));
	state->window = 0;
	state->dragging_widget = 0;
	wg->data = (u8*)state;
	wg->type = WIDGET_MAIN;
	wg->x = 0;
	wg->y = 0;
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
	qui_widget* wg = mem_alloc(sizeof(qui_widget));
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

		//if (parent->type == WIDGET_FIXED_CONTAINER || parent->type == WIDGET_SIZE_CONTAINER || parent->type == WIDGET_FLEX_CONTAINER) {
			//log_assert(parent->children.length == 0, "Fixed, Size and Flex containers can only contain 1 child");
		//}
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

	if (el->type == WIDGET_BUTTON) _qui_render_button(el);
	if (el->type == WIDGET_TOOLBAR) _qui_render_toolbar(el);
	if (el->type == WIDGET_TOOLBAR_ITEM) _qui_render_toolbar_item(el);
	if (el->type == WIDGET_TOOLBAR_ITEM_OPTION) _qui_render_toolbar_item_option(el);
	if (el->type == WIDGET_LABEL) _qui_render_label(el);
	if (el->type == WIDGET_DROPDOWN) _qui_render_dropdown(el);
	if (el->type == WIDGET_DROPDOWN_OPTION) _qui_render_dropdown_option(el);
	if (el->type == WIDGET_TABCONTROL) _qui_render_tabcontrol(el);
	if (el->type == WIDGET_TABCONTROL_PANEL) _qui_render_tabcontrol_panel(el);
	if (el->type == WIDGET_SCROLL) _qui_render_scroll(el);
	if (el->type == WIDGET_SCROLL_BUTTON) _qui_render_scroll_button(el);
	if (el->type == WIDGET_SCROLL_BAR) _qui_render_scroll_bar(el);
	if (el->type == WIDGET_IMAGE_PANEL) _qui_render_image_panel(el);

	if (el->type == WIDGET_VERTICAL_LAYOUT/* || el->type == WIDGET_MAIN*/) _qui_render_vertical_layout(el);
	if (el->type == WIDGET_FIXED_CONTAINER) _qui_render_fixed_container(el);
	if (el->type == WIDGET_SIZE_CONTAINER) _qui_render_size_container(el);
	if (el->type == WIDGET_FLEX_CONTAINER) _qui_render_flex_container(el);
	if (el->type == WIDGET_HORIZONTAL_LAYOUT) _qui_render_horizontal_layout(el);
	state->scissor_index++;
	for (s32 i = 0; i < el->children.length; i++) {
		qui_widget* w = *(qui_widget**)array_at(&el->children, i);
		_qui_render_widget(state, w, draw_special);
	}
	state->scissor_index--;
}

qui_widget* _qui_find_parent_of_type(qui_widget* widget, qui_widget_type type) {
	qui_widget* parent = widget->parent;
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
	if (el->type == WIDGET_VERTICAL_LAYOUT/* || el->type == WIDGET_MAIN*/) _qui_update_vertical_layout(el);
	if (el->type == WIDGET_HORIZONTAL_LAYOUT) _qui_update_horizontal_layout(el);

	state->scissor_index++;
	for (s32 i = 0; i < el->children.length; i++) {
		qui_widget* w = *(qui_widget**)array_at(&el->children, i);
		_qui_update_widget(state, w, update_special);
	}
	state->scissor_index--;

	if (el->type == WIDGET_BUTTON) _qui_update_button(state, el);
	if (el->type == WIDGET_TOOLBAR) _qui_update_toolbar(el);
	if (el->type == WIDGET_TOOLBAR_ITEM) _qui_update_toolbar_item(el);
	if (el->type == WIDGET_TOOLBAR_ITEM_OPTION) _qui_update_toolbar_item_option(el);
	if (el->type == WIDGET_LABEL) _qui_update_label(el);
	if (el->type == WIDGET_DROPDOWN) _qui_update_dropdown(state, el);
	if (el->type == WIDGET_DROPDOWN_OPTION) _qui_update_dropdown_option(el);
	if (el->type == WIDGET_TABCONTROL) _qui_update_tabcontrol(el);
	if (el->type == WIDGET_TABCONTROL_PANEL) _qui_update_tabcontrol_panel(state, el);
	if (el->type == WIDGET_SCROLL_BUTTON) _qui_update_scroll_button(state, el);
	if (el->type == WIDGET_SCROLL_BAR) _qui_update_scroll_bar(el);
	if (el->type == WIDGET_IMAGE_PANEL) _qui_update_image_panel(el);

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