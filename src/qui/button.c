void _qui_update_button(qui_state* main_state, qui_widget* el) {
	qui_button* data = ((qui_button*)el->data);
	if (el->parent->type == WIDGET_HORIZONTAL_LAYOUT) {
		if (data->text) {
			el->width = renderer->calculate_text_width(global_ui_context.font_small, 
				(data->text)) + (BUTTON_PADDING_W*2);
		}
		else {
			el->width = el->height;
		}
	}

	vec4 actual_area = main_state->scissor_stack[main_state->scissor_index];
	if (_qui_mouse_interacts(main_state, actual_area)) {
		if (!data->is_toggle) {
			data->state = HOVERED;
			if (is_left_down()) data->state = DOWN;
		}
		else {
			#if 1
			if (is_left_clicked()) {
				if (data->released) {
					if (data->is_toggle && data->state == DOWN) {
						data->state = HOVERED;
					}
					else {
						data->state = DOWN;
					}
				}

				data->released = false;
			}
			else {
				data->released = true;
				if (!data->is_toggle || (data->is_toggle && data->state == IDLE)) {
					data->state = HOVERED;
				}
			}
			#endif
		}

		if (is_left_clicked()) {
			// Handle event.
		}
	}
	else {
		if (!data->is_toggle || (data->is_toggle && data->state == HOVERED)) data->state = IDLE;
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

qui_widget* qui_create_toggle_button_with_icon(qui_widget* qui, char* path)
{
	qui_widget* wg = qui_create_button(qui, 0);
	((qui_button*)wg->data)->icon = assets_load_image_from_file(path);
	((qui_button*)wg->data)->is_toggle = true;
	wg->margin_x = 2;
	wg->margin_y = 2;
	return wg;
}


qui_widget* qui_create_button_with_icon(qui_widget* qui, char* path)
{
	qui_widget* wg = qui_create_button(qui, 0);
	((qui_button*)wg->data)->icon = assets_load_image_from_file(path);
	wg->margin_x = 2;
	wg->margin_y = 2;
	return wg;
}

qui_widget* qui_create_button(qui_widget* qui, char* text)
{
	log_assert(qui->type == WIDGET_VERTICAL_LAYOUT || qui->type == WIDGET_HORIZONTAL_LAYOUT, "Button can only be added to vertical or horizontal layout");
	qui_widget* wg = _qui_create_empty_widget(qui);
	qui_button* data = mem_alloc(sizeof(qui_button));
	data->text = text;
	data->icon = 0;
	data->is_toggle = false;
	data->state = IDLE;
	wg->data = (u8*)data;
	wg->type = WIDGET_BUTTON;
	wg->height = global_ui_context.font_small->px_h + (BUTTON_PADDING_H*2);
	wg->margin_x = INTERACTIVE_ELEMENT_MARGIN_W;
	wg->margin_y = INTERACTIVE_ELEMENT_MARGIN_H;
	return wg;
}