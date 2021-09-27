void _qui_update_button(qui_widget* el) {
	if (mouse_interacts(el->x, el->y, el->width, el->height)) {
		((qui_button*)el->data)->state = HOVERED;
		if (is_left_down()) {
			((qui_button*)el->data)->state = DOWN;
		}
		if (is_left_clicked()) {
			// Handle event.
		}
	}
	else {
		((qui_button*)el->data)->state = IDLE;
	}
}


void _qui_render_button(qui_widget* el) {
	char* text = ((qui_button*)el->data)->text;
	int state = ((qui_button*)el->data)->state;
	s32 tw = renderer->calculate_text_width(global_ui_context.font_small, text);

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
		
	renderer->render_text(global_ui_context.font_small, 
		el->x+(el->width/2)-(tw/2),
		el->y+(el->height/2)-(global_ui_context.font_small->px_h/2), 
		text, active_ui_style.widget_text);
}

qui_widget* qui_create_button(qui_widget* qui, char* text)
{
	log_assert(qui->type == WIDGET_VERTICAL_LAYOUT, "Button can only be added to vertical or horizontal layout");
	qui_widget* wg = _qui_create_empty_widget(qui);
	qui_button* data = mem_alloc(sizeof(qui_button));
	data->text = text;
	data->state = IDLE;
	wg->data = (u8*)data;
	wg->type = WIDGET_BUTTON;
	wg->height = global_ui_context.font_small->px_h + (BUTTON_PADDING_H*2);
	wg->x = 50;
	wg->y = 50;
	wg->margin_x = INTERACTIVE_ELEMENT_MARGIN_W;
	wg->margin_y = INTERACTIVE_ELEMENT_MARGIN_H;
	return wg;
}