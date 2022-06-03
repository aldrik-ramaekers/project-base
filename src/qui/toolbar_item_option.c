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

void _qui_update_toolbar_item_option(qui_state* main_state, qui_widget* el) {
	el->height = main_state->font_default->px_h + (TOOLBAR_ITEM_PADDING_OPTION_H*2);

	qui_widget_state *state = &(((qui_toolbar_item*)el->data)->state);
	if (!_qui_toolbar_item_option_is_visible(el)) {
		*state = IDLE;
		return;
	}

	if (mouse_interacts(el->x, el->y, el->width, el->height)) {
		if (*state == IDLE) *state = HOVERED;
		if (is_left_clicked()) {
			// If this is a clickable button, not an expansion, close everything.
			if (!el->children.length) {
				qui_widget* top_parent = _qui_find_parent_of_type(el, WIDGET_TOOLBAR_ITEM);
				if (top_parent) _qui_close_entire_toolbar_item(top_parent);
			}
			else {
				// Throw event here.
			}
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


void _qui_render_toolbar_item_option(qui_state* main_state, qui_widget* el) {
	if (!_qui_toolbar_item_option_is_visible(el)) {
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
	
	#define TOOLBAR_ITEM_OPTION_TEXT_PAD_LEFT 30
	#define TOOLBAR_ITEM_OPTION_TEXT_PAD_RIGHT 100
	s32 text_width_available = (el->width - TOOLBAR_ITEM_OPTION_TEXT_PAD_LEFT - TOOLBAR_ITEM_OPTION_TEXT_PAD_RIGHT);

	// Draw text.
	renderer->render_text_ellipsed(main_state->font_default, 
		el->x+TOOLBAR_ITEM_OPTION_TEXT_PAD_LEFT,
		el->y+(el->height/2)-(main_state->font_default->px_h/2), 
		text_width_available, text, active_ui_style.widget_text);

	// Draw icon.
	if (data->icon) {
		const s32 ICON_PAD = 4;
		s32 icon_s = el->height - (ICON_PAD*2);
		renderer->render_image_tint(data->icon, el->x + (TOOLBAR_ITEM_OPTION_TEXT_PAD_LEFT/2) - (icon_s/2), el->y + ICON_PAD, icon_s, icon_s, active_ui_style.widget_interactive_image_tint);
	}

	// Draw collapse arrow.
	if (el->children.length) {
		s32 arrow_s = el->height/3;
		renderer->render_triangle(el->x + el->width - (arrow_s*2), el->y + (el->height/2)-(arrow_s/2), arrow_s, arrow_s, active_ui_style.collapse_color, TRIANGLE_RIGHT);
	}

	_qui_render_toolbar_item_options_bounds(el);
}

qui_widget* qui_create_toolbar_item_option_with_icon(qui_widget* qui, char* text, char* icon)
{
	qui_widget* wg = qui_create_toolbar_item_option(qui, text);
	((qui_toolbar_item*)wg->data)->icon = assets_load_image_from_file(icon);
	return wg;
}

qui_widget* qui_create_toolbar_item_option(qui_widget* qui, char* text)
{
	log_assert(qui->type == WIDGET_TOOLBAR_ITEM ||
			   qui->type == WIDGET_TOOLBAR_ITEM_OPTION, "Toolbar item option can only be added to toolbar item or toolbar item option");

	qui_widget* wg = _qui_create_empty_widget(qui);
	qui_toolbar_item* data = mem_alloc(sizeof(qui_toolbar_item));
	data->text = text;
	data->state = IDLE;
	data->icon = 0;
	wg->data = (u8*)data;
	wg->type = WIDGET_TOOLBAR_ITEM_OPTION;
	wg->width = TOOLBAR_ITEM_OPTION_W;

	if (qui->type == WIDGET_TOOLBAR_ITEM) {
		qui_widget* master_widget = _qui_find_parent_of_type(wg, WIDGET_MAIN);
		if (master_widget) array_push(&master_widget->special_children, (uint8_t*)&wg);
		else log_assert(0, "QUI needs a master element created by qui_setup()");
	}
	return wg;
}