void _qui_update_dropdown(qui_widget* el) {
	qui_dropdown* data = ((qui_dropdown*)el->data);
	if (mouse_interacts(el->x, el->y, el->width, el->height)) {
		if (data->state == IDLE) data->state = HOVERED;
		if (is_left_clicked()) {
			data->state = OPEN;
		}
	}

	el->width = 0;
	s32 offsety = 0;
	for (s32 i = 0; i < el->children.length; i++) {
		qui_widget* w = *(qui_widget**)array_at(&el->children, i);
		qui_button* data = (qui_button*)w->data;
		w->x = el->x;
		w->y = el->y + offsety;
		offsety += w->height;

		s32 tw = renderer->calculate_text_width(global_ui_context.font_small, data->text);
		s32 totalw = tw + (BUTTON_PADDING_W*2);
		if (totalw > el->width) el->width = totalw;
	}
}

void _qui_render_dropdown_options_bounds(qui_widget* el) {
	if (el->children.length == 0) return;
	qui_widget* first_option = *(qui_widget**)array_at(&el->children, 0);
	s32 height_per_item = first_option->height;
	s32 total_height_of_options = el->children.length*height_per_item;

	renderer->render_rectangle_outline(first_option->x-1, first_option->y-1, el->width+2, total_height_of_options+2, 1, active_ui_style.widget_border_outter_static);
}

void _qui_render_dropdown(qui_widget* el) {
	qui_dropdown* data = ((qui_dropdown*)el->data);
	int state = data->state;
	color outter = active_ui_style.widget_border_outter_idle;
	color inner = active_ui_style.widget_border_inner_idle;
	color background = active_ui_style.widget_background_interactive_idle;

	if (state == HOVERED) {
		outter = active_ui_style.widget_border_outter_hovered;
		inner = active_ui_style.widget_border_inner_hovered;
		background = active_ui_style.widget_background_interactive_hovered;
	}
	else if (state == DOWN) {
		outter = active_ui_style.widget_border_outter_highlighted;
		inner = active_ui_style.widget_border_inner_highlighted;
		background = active_ui_style.widget_background_interactive_highlighted;
	}

	renderer->render_rounded_rectangle(el->x,
		el->y,el->width, 
		el->height, 
		outter, 5.0f, 0);

	renderer->render_rounded_rectangle(el->x,
		el->y,
		el->width, 
		el->height,
		inner, 5.0f, 1);

	renderer->render_rounded_rectangle(el->x,
		el->y,
		el->width, 
		el->height,
		background, 5.0f, 2);

	s32 offset_y = (el->height/2)-(global_ui_context.font_small->px_h/2);
	#if 0
	if (data->text) {
		char* text = ((qui_dropdown*)el->data)->text;
		renderer->render_text(global_ui_context.font_small, 
			el->x+offset_y,
			el->y+offset_y, 
			text, active_ui_style.widget_text);
	}
	#endif

	s32 triangle_s = global_ui_context.font_small->px_h;
	renderer->render_triangle(el->x + el->width - triangle_s - offset_y, el->y + offset_y, 
		triangle_s, triangle_s, active_ui_style.collapse_color, TRIANGLE_DOWN);

	if (state == OPEN)_qui_render_dropdown_options_bounds(el);
}

qui_widget* qui_create_dropdown(qui_widget* qui)
{
	log_assert(qui->type == WIDGET_VERTICAL_LAYOUT || qui->type == WIDGET_HORIZONTAL_LAYOUT, "Dropdown can only be added to vertical or horizontal layout");
	qui_widget* wg = _qui_create_empty_widget(qui);
	qui_dropdown* data = mem_alloc(sizeof(qui_dropdown));
	wg->data = (u8*)data;
	wg->type = WIDGET_DROPDOWN;
	wg->height = global_ui_context.font_small->px_h + (BUTTON_PADDING_H*2);
	wg->margin_x = INTERACTIVE_ELEMENT_MARGIN_W;
	wg->margin_y = INTERACTIVE_ELEMENT_MARGIN_H;
	return wg;
}