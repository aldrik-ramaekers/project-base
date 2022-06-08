
void _qui_update_scroll_button(qui_state* main_state, qui_widget* el) {
	qui_scroll_button_widget* data = (qui_scroll_button_widget*)el->data;

	vec4 actual_area = main_state->scissor_stack[main_state->scissor_index];
	if (_qui_mouse_interacts(main_state, actual_area)) {
		data->state = HOVERED;
		if (is_left_down()) {
			data->state = DOWN;
		}
	}
	else {
		data->state = IDLE;
	}
}

void _qui_render_scroll_button(qui_state* main_state, qui_widget* el) {
	color outter = active_ui_style.widget_border_outter_idle;
	color inner = active_ui_style.widget_border_inner_idle;
	color background = active_ui_style.widget_background_interactive_idle;

	qui_scroll_button_widget* data = (qui_scroll_button_widget*)el->data;

	if (data->state == HOVERED) {
		outter = active_ui_style.widget_border_outter_hovered;
		inner = active_ui_style.widget_border_inner_hovered;
		background = active_ui_style.widget_background_interactive_hovered;
	}
	else if (data->state == DOWN) {
		outter = active_ui_style.widget_border_outter_highlighted;
		inner = active_ui_style.widget_border_inner_highlighted;
		background = active_ui_style.widget_background_interactive_highlighted;
	}

	renderer->render_rectangle(el->x,
		el->y,el->width, 
		el->height, 
		outter);

	renderer->render_rectangle(el->x+1,
		el->y+1,
		el->width-2, 
		el->height-2,
		inner);

	renderer->render_rectangle(el->x+2,
		el->y+2,
		el->width-4, 
		el->height-4,
		background);

	s32 triangle_size = el->width/2;
	renderer->render_triangle(
			el->x+(el->width/2)-(triangle_size/2), 
			el->y+(el->height/2)-(triangle_size/2), 
			triangle_size, triangle_size, active_ui_style.collapse_color, data->is_up ? TRIANGLE_UP : TRIANGLE_DOWN);
}

void _qui_create_scroll_button(qui_widget* qui, bool is_up) {
	log_assert(qui->type == WIDGET_VERTICAL_LAYOUT || qui->type == WIDGET_HORIZONTAL_LAYOUT, "Scroll button can only be added to vertical or horizontal layout");
	qui_widget* el = _qui_create_empty_widget(qui);
	el->type = WIDGET_SCROLL_BUTTON;
	el->width = SCROLLBAR_W;
	el->height = SCROLLBAR_W;
	qui_scroll_button_widget* data = (qui_scroll_button_widget*)_qui_allocate(qui, sizeof(qui_scroll_button_widget));
	data->is_up = is_up;
	data->state = IDLE;
	el->data = (u8*)data;
}