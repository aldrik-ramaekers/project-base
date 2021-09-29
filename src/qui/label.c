void _qui_update_label(qui_widget* el) {
	qui_label* data = ((qui_label*)el->data);
	el->width = renderer->calculate_text_width(global_ui_context.font_small, 
				(data->text)) + (LABEL_PADDING_W*2);
}


void _qui_render_label(qui_widget* el) {
	qui_label* data = ((qui_label*)el->data);

	if (data->text) {
		char* text = data->text;
		s32 tw = renderer->calculate_text_width(global_ui_context.font_small, text);
		renderer->render_text(global_ui_context.font_small, 
			el->x+(el->width/2)-(tw/2),
			el->y+(el->height/2)-(global_ui_context.font_small->px_h/2), 
			text, active_ui_style.widget_text);
	}
}

qui_widget* qui_create_label(qui_widget* qui, char* text)
{
	log_assert(qui->type == WIDGET_VERTICAL_LAYOUT || qui->type == WIDGET_HORIZONTAL_LAYOUT, "Label can only be added to vertical or horizontal layout");
	qui_widget* wg = _qui_create_empty_widget(qui);
	qui_label* data = mem_alloc(sizeof(qui_label));
	data->text = text;
	wg->data = (u8*)data;
	wg->type = WIDGET_LABEL;
	wg->height = global_ui_context.font_small->px_h + (LABEL_PADDING_H*2);
	wg->margin_x = INTERACTIVE_ELEMENT_MARGIN_W;
	wg->margin_y = INTERACTIVE_ELEMENT_MARGIN_H;
	return wg;
}