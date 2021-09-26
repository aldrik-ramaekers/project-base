void _qui_close_entire_toolbar_item(qui_widget* el) {
	((qui_toolbar_item*)(el->data))->state = IDLE;
	for (s32 i = 0; i < el->children.length; i++) {
		qui_widget* w = *(qui_widget**)array_at(&el->children, i);
		((qui_toolbar_item*)(w->data))->state = IDLE;
		_qui_close_entire_toolbar_item(w);
	}
}

void _qui_update_toolbar_item(qui_widget* el) {
	el->width = renderer->calculate_text_width(global_ui_context.font_small, 
		((qui_toolbar_item*)el->data)->text) + (TOOLBAR_ITEM_PADDING_W*2);
	qui_widget_state *state = &(((qui_toolbar_item*)el->data)->state);
	if (mouse_interacts(el->x, el->y, el->width, el->height)) {
		if (*state == IDLE) *state = HOVERED;
		if (is_left_clicked()) {
			*state = OPEN;
		}
	}
	else if (*state != OPEN) {
		*state = IDLE;
	}

	s32 offsety = 0;
	for (s32 i = 0; i < el->children.length; i++) {
		qui_widget* w = *(qui_widget**)array_at(&el->children, i);
		w->y = el->y + TOOLBAR_H + offsety;
		w->x = el->x;
		offsety += w->height;
	}
}

void _qui_render_toolbar_item_options_bounds(qui_widget* el) {
	if (el->children.length == 0) return;
	qui_widget* first_option = *(qui_widget**)array_at(&el->children, 0);
	s32 height_per_item = first_option->height;
	s32 total_height_of_options = el->children.length*height_per_item;

	renderer->render_rectangle_outline(first_option->x-1, first_option->y-1, TOOLBAR_ITEM_OPTION_W+2, total_height_of_options+2, 1, active_ui_style.widget_border_outter_static);
}

void _qui_render_toolbar_item(qui_widget* el) {
	char* text = ((qui_toolbar_item*)el->data)->text;
	int state = ((qui_toolbar_item*)el->data)->state;
	s32 tw = renderer->calculate_text_width(global_ui_context.font_small, text);

	color background = active_ui_style.widget_background_static;

	if (state == HOVERED || state == OPEN) {
		background = active_ui_style.widget_background_interactive_hovered;
	}
	else if (state == DOWN) {
		background = active_ui_style.widget_background_interactive_highlighted;
	}

	renderer->render_rectangle(el->x,
		el->y,el->width, 
		el->height, 
		background);
		
	renderer->render_text(global_ui_context.font_small, 
		el->x+(el->width/2)-(tw/2),
		el->y+(el->height/2)-(global_ui_context.font_small->px_h/2), 
		text, active_ui_style.widget_text);

	if (state == OPEN) _qui_render_toolbar_item_options_bounds(el);
}

qui_widget* qui_create_toolbar_item(qui_widget* toolbar, char* text)
{
	qui_widget* wg = _qui_create_empty_widget(toolbar);
	qui_toolbar_item* data = mem_alloc(sizeof(qui_toolbar_item));
	data->text = text;
	data->state = IDLE;
	wg->data = (u8*)data;
	wg->type = WIDGET_TOOLBAR_ITEM;
	wg->height = TOOLBAR_H-1;
	return wg;
}