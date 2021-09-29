void _qui_update_button(qui_widget* el) {
	if (el->parent->type == WIDGET_HORIZONTAL_LAYOUT) {
		if (((qui_button*)el->data)->text) {
			el->width = renderer->calculate_text_width(global_ui_context.font_small, 
				((qui_button*)el->data)->text) + (BUTTON_PADDING_W*2);
		}
		else {
			el->width = el->height;
		}
	}

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
	qui_button* data = ((qui_button*)el->data);
	int state = data->state;
	color outter = active_ui_style.widget_border_outter_idle;
	color inner = active_ui_style.widget_border_inner_idle;
	color background = active_ui_style.widget_background_interactive_idle;

	if (data->icon) {
		outter = rgba(0,0,0,0);
		inner = rgba(0,0,0,0);
		background = rgba(0,0,0,0);
	}

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
	
	if (data->text) {
		char* text = ((qui_button*)el->data)->text;
		s32 tw = renderer->calculate_text_width(global_ui_context.font_small, text);
		renderer->render_text(global_ui_context.font_small, 
			el->x+(el->width/2)-(tw/2),
			el->y+(el->height/2)-(global_ui_context.font_small->px_h/2), 
			text, active_ui_style.widget_text);
	}
	else if (data->icon) {
		#define IMAGE_PAD 5
		s32 icon_s = el->width - (IMAGE_PAD*2);
		renderer->render_image(((qui_button*)el->data)->icon, el->x + IMAGE_PAD, el->y + IMAGE_PAD, icon_s, icon_s);
	}
}

qui_widget* qui_create_button_with_icon(qui_widget* qui, char* path)
{
	log_assert(qui->type == WIDGET_VERTICAL_LAYOUT || qui->type == WIDGET_HORIZONTAL_LAYOUT, "Button can only be added to vertical or horizontal layout");
	qui_widget* wg = _qui_create_empty_widget(qui);
	qui_button* data = mem_alloc(sizeof(qui_button));
	data->text = 0;
	data->icon = assets_load_image_from_file(path);
	data->state = IDLE;
	wg->data = (u8*)data;
	wg->type = WIDGET_BUTTON;
	wg->height = global_ui_context.font_small->px_h + (BUTTON_PADDING_H*2);
	wg->x = 50;
	wg->y = 50;
	wg->margin_x = INTERACTIVE_ELEMENT_MARGIN_W/3;
	wg->margin_y = INTERACTIVE_ELEMENT_MARGIN_H/3;
	return wg;
}

qui_widget* qui_create_button(qui_widget* qui, char* text)
{
	log_assert(qui->type == WIDGET_VERTICAL_LAYOUT || qui->type == WIDGET_HORIZONTAL_LAYOUT, "Button can only be added to vertical or horizontal layout");
	qui_widget* wg = _qui_create_empty_widget(qui);
	qui_button* data = mem_alloc(sizeof(qui_button));
	data->text = text;
	data->icon = 0;
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