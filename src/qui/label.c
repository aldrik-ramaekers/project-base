void _qui_update_label(qui_widget* el) {
	qui_label* data = ((qui_label*)el->data);
	s32 tw = renderer->calculate_text_width(global_ui_context.font_small, 
			(data->text)) + (LABEL_PADDING_W*2);
	
	el->width = tw;
	if (el->width > el->parent->width) el->width = el->parent->width;
}


void _qui_render_label(qui_widget* el) {
	qui_label* data = ((qui_label*)el->data);

	if (data->text) {
		char* text = data->text;
		s32 tw = renderer->calculate_text_width(global_ui_context.font_small, text);

		if (el->width == tw) {
			renderer->render_text(global_ui_context.font_small, 
				el->x,
				el->y + (el->height/2)-(global_ui_context.font_small->px_h/2), // Label is single line so center it on y-axis.
				text, 
				active_ui_style.widget_text);
		}
		else {
			// TODO: we should really have a function to calculate the height of cutoff text before rendering it..
			el->height = renderer->render_text_cutoff(global_ui_context.font_small, 
				el->x,
				el->y,
				text, 
				active_ui_style.widget_text, el->width);
		}
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
	wg->height = global_ui_context.font_small->px_h;
	wg->margin_x = INTERACTIVE_ELEMENT_MARGIN_W;
	wg->margin_y = INTERACTIVE_ELEMENT_MARGIN_H;
	return wg;
}