void _qui_close_dropdown(qui_widget* el) {
	qui_widget* dropdown = el->parent;
	((qui_dropdown*)dropdown->data)->state = IDLE;
}

bool _qui_is_dropdown_option_sibling_hovered(qui_widget* el) {
	if (!el->parent) return false;
	for (s32 i = 0; i < el->parent->children.length; i++) {
		qui_widget* w = *(qui_widget**)array_at(&el->parent->children, i);
		if (w == el) continue;
		if (((qui_toolbar_item*)w->data)->state == HOVERED) return true;
	}
	return false;
}

bool _qui_dropdown_option_is_visible(qui_widget* el) {
	if (!el->parent) return false;
	return ((qui_dropdown*)(el->parent->data))->state == OPEN;
}

void _qui_update_dropdown_option(qui_widget* el) {
	el->width = el->parent->width;

	qui_widget_state *state = &(((qui_toolbar_item*)el->data)->state);
	if (!_qui_dropdown_option_is_visible(el)) {
		*state = IDLE;
		return;
	}

	if (mouse_interacts(el->x, el->y, el->width, el->height)) {
		if (*state == IDLE) *state = HOVERED;
		if (is_left_clicked()) {
			// Throw event here
			_qui_close_dropdown(el);
		}
	}
	else if (_qui_is_dropdown_option_sibling_hovered(el) ) {
		*state = IDLE;
	}
}

void _qui_render_dropdown_option(qui_widget* el) {
	if (!_qui_dropdown_option_is_visible(el)) {
		return;
	}
	
	qui_toolbar_item* data = ((qui_toolbar_item*)el->data);
	char* text = ((qui_toolbar_item*)el->data)->text;
	int state = ((qui_toolbar_item*)el->data)->state;

	color background = active_ui_style.widget_background_static;

	color outter = background;
	if (state == HOVERED || state == OPEN || state == DOWN) {
		background = active_ui_style.widget_background_interactive_selected_option;
		outter = active_ui_style.widget_border_outter_highlighted;
	}

	renderer->render_rectangle(el->x,
		el->y,el->width,
		el->height,
		background);
	renderer->render_rectangle_outline(el->x,
		el->y,el->width,
		el->height,
		1, outter);
	
	// Draw text.
	const s32 DROPDOWN_OPTION_TEXT_OFFSET = 10;
	renderer->render_text(global_ui_context.font_small, 
		el->x + DROPDOWN_OPTION_TEXT_OFFSET, el->y+(el->height/2)-(global_ui_context.font_small->px_h/2), text, active_ui_style.widget_text);
}

qui_widget* qui_create_dropdown_option(qui_widget* qui, char* text)
{
	log_assert(qui->type == WIDGET_DROPDOWN, "Dropdown option option can only be added to dropdown");

	qui_widget* wg = _qui_create_empty_widget(qui);
	qui_button* data = mem_alloc(sizeof(qui_button));
	data->text = text;
	data->state = IDLE;
	data->icon = 0;
	wg->data = (u8*)data;
	wg->type = WIDGET_DROPDOWN_OPTION;
	wg->width = 0;
	wg->height = global_ui_context.font_small->px_h + (TOOLBAR_ITEM_PADDING_OPTION_H*2);

	qui_widget* master_widget = _qui_find_parent_of_type(wg, WIDGET_MAIN);
	if (master_widget) array_push(&master_widget->special_children, (uint8_t*)&wg);
	else log_assert(0, "QUI needs a master element created by qui_setup()");
	return wg;
}