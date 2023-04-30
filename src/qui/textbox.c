void _qui_update_textbox(qui_state* main_state, qui_widget* el) {
	qui_textbox* data = ((qui_textbox*)el->data);

	vec4 actual_area = main_state->scissor_stack[main_state->scissor_index];
	if (_qui_mouse_interacts(main_state, actual_area)) {
		if (is_left_clicked()) {
			data->state = OPEN;
			_global_keyboard.take_input = true;
		}
	}
	else {
		if (data->state == OPEN && is_left_clicked()) {
			data->state = IDLE;
			_global_keyboard.take_input = false;
		} 
	}

	strncpy(data->buffer, _global_keyboard.input_text, TEXTBOX_BUFFER_SIZE);
}

static void _draw_box(qui_state* main_state, qui_widget* el)
{
	qui_textbox* data = ((qui_textbox*)el->data);
	int state = data->state;
	color outter = active_ui_style.widget_border_outter_idle;
	color inner = active_ui_style.widget_border_inner_idle;
	color background = active_ui_style.widget_textbox_background;

	if (state == OPEN) {
		inner = outter;
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
}

void _qui_render_textbox(qui_state* main_state, qui_widget* el) {
	qui_textbox* data = ((qui_textbox*)el->data);

	_draw_box(main_state, el);

	int texty = el->y + (el->height/2) - (main_state->font_default->px_h/2);
	int textx = el->x + el->margin_x;
	//int logical_width = el->width - (el->margin_x*2);
	//int textw = renderer->calculate_text_width(main_state->font_default, data->buffer);

	//_qui_render_set_scissor(main_state, el, false);

	renderer->render_text(main_state->font_default, textx, texty, data->buffer, rgb(0,0,0));

	//_qui_render_scissor_pop(main_state);
}

qui_widget* qui_create_textbox(qui_widget* qui)
{
	log_assert(qui->type == WIDGET_VERTICAL_LAYOUT || qui->type == WIDGET_HORIZONTAL_LAYOUT, "Textbox can only be added to vertical or horizontal layout");
	qui_widget* wg = _qui_create_empty_widget(qui);
	qui_dropdown* data = (qui_dropdown*)_qui_allocate(qui, sizeof(qui_textbox));
	data->state = IDLE;
	wg->data = (u8*)data;
	wg->type = WIDGET_TEXTBOX;
	wg->height = (BUTTON_PADDING_H*2);
	wg->width = 200;
	wg->margin_x = INTERACTIVE_ELEMENT_MARGIN_W;
	wg->margin_y = INTERACTIVE_ELEMENT_MARGIN_H;
	return wg;
}