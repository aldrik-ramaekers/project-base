void _qui_render_toolbar_item_options_bounds(qui_widget* el);
void _qui_close_entire_toolbar_item(qui_widget* el);

bool _qui_is_toolbar_item_option_sibling_hovered(qui_widget* el) {
	if (!el->parent) return false;
	for (s32 i = 0; i < el->parent->children.length; i++) {
		qui_widget* w = *(qui_widget**)array_at(&el->parent->children, i);
		if (w == el) continue;
		if (((qui_toolbar_item*)w->data)->state == HOVERED) return true;
	}
	return false;
}

bool _qui_is_toolbar_item_option_child_hovered(qui_widget* el) {
	for (s32 i = 0; i < el->children.length; i++) {
		qui_widget* w = *(qui_widget**)array_at(&el->children, i);
		if (((qui_toolbar_item*)w->data)->state == HOVERED) return true;
	}
	return false;
}

bool _qui_toolbar_item_option_is_visible(qui_widget* el) {
	if (!el->parent) return false;
	if (el->parent->type == WIDGET_TOOLBAR_ITEM) {
		return ((qui_toolbar_item*)(el->parent->data))->state == OPEN;
	}
	else if (el->parent->type == WIDGET_TOOLBAR_ITEM_OPTION) {
		return ((qui_toolbar_item*)(el->parent->data))->state == HOVERED;
	}
	return false;
}

void _qui_update_toolbar_item_option(qui_widget* el) {
	qui_widget_state *state = &(((qui_toolbar_item*)el->data)->state);
	if (!_qui_toolbar_item_option_is_visible(el)) {
		*state = IDLE;
		return;
	}

	if (mouse_interacts(el->x, el->y, el->width, el->height)) {
		if (*state == IDLE) *state = HOVERED;
		if (is_left_clicked()) {
			// Throw event here.

			qui_widget* top_parent = _qui_find_parent_of_type(el, WIDGET_TOOLBAR_ITEM);
			if (top_parent) _qui_close_entire_toolbar_item(top_parent);
		}
	}
	else if (_qui_is_toolbar_item_option_sibling_hovered(el) || !_qui_is_toolbar_item_option_child_hovered(el)) {
		*state = IDLE;
	}

	s32 offsety = 0;
	for (s32 i = 0; i < el->children.length; i++) {
		qui_widget* w = *(qui_widget**)array_at(&el->children, i);
		w->y = el->y + offsety;
		w->x = el->x + el->width;
		offsety += w->height;
	}
}


void _qui_render_toolbar_item_option(qui_widget* el) {
	if (!_qui_toolbar_item_option_is_visible(el)) {
		return;
	}
	
	char* text = ((qui_toolbar_item*)el->data)->text;
	int state = ((qui_toolbar_item*)el->data)->state;
	s32 tw = renderer->calculate_text_width(global_ui_context.font_small, text);

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
		
	renderer->render_text(global_ui_context.font_small, 
		el->x+(el->width/2)-(tw/2),
		el->y+(el->height/2)-(global_ui_context.font_small->px_h/2), 
		text, active_ui_style.widget_text);

	if (state == HOVERED) _qui_render_toolbar_item_options_bounds(el);
}

qui_widget* qui_create_toolbar_item_option(qui_widget* qui, char* text)
{
	qui_widget* wg = _qui_create_empty_widget(qui);
	qui_button* data = mem_alloc(sizeof(qui_button));
	data->text = text;
	data->state = IDLE;
	wg->data = (u8*)data;
	wg->type = WIDGET_TOOLBAR_ITEM_OPTION;
	wg->width = TOOLBAR_ITEM_OPTION_W;
	wg->height = global_ui_context.font_small->px_h + (TOOLBAR_ITEM_PADDING_OPTION_H*2);

	qui_widget* master_widget = _qui_find_parent_of_type(qui, WIDGET_MAIN);
	if (master_widget) array_push(&master_widget->special_children, (uint8_t*)&wg);
	else log_assert(0, "QUI is need a master element created by qui_setup()");
	return wg;
}