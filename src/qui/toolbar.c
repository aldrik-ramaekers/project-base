void _qui_close_entire_toolbar(qui_widget* el) {
	qui_widget* toolbar_el = 0;
	if (el->type == WIDGET_TOOLBAR) toolbar_el = el;
	else toolbar_el = _qui_find_parent_of_type(el, WIDGET_TOOLBAR);
	log_assert(toolbar_el, "Toolbar item option is not under a toolbar");
	el = toolbar_el;
	for (s32 i = 0; i < el->children.length; i++) {
		qui_widget* w = *(qui_widget**)array_at(&el->children, i);
		log_assert(w->type == WIDGET_TOOLBAR_ITEM, "Toolbar can only have toolbar item widgets as children");
		_qui_close_entire_toolbar_item(w);
	}
}

qui_widget* qui_create_toolbar(qui_widget* qui)
{
	log_assert(qui->type == WIDGET_VERTICAL_LAYOUT || qui->type == WIDGET_HORIZONTAL_LAYOUT, "Toolbar can only be added to vertical or horizontal layout");
	qui_widget* wg = _qui_create_empty_widget(qui);
	wg->width = 500;
	wg->height = TOOLBAR_H;
	wg->type = WIDGET_TOOLBAR;
	return wg;
}

void _qui_render_toolbar(qui_state* main_state, qui_widget* el) {
	renderer->render_rectangle(el->x, el->y, el->width, el->height, active_ui_style.widget_background_static);
	renderer->render_rectangle(el->x, el->y + el->height - 1, el->width, 1, active_ui_style.widget_border_outter_static);
}

void _qui_update_toolbar(qui_state* main_state, qui_widget* el) {
	#define TOOLBAR_ITEM_OFFSETX (10)
	s32 offsetx = 0;
	for (s32 i = 0; i < el->children.length; i++) {
		qui_widget* w = *(qui_widget**)array_at(&el->children, i);
		w->x = TOOLBAR_ITEM_OFFSETX + el->x + offsetx;
		w->y = el->y;
		offsetx += w->width;
	}
}