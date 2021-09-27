void _qui_close_entire_toolbar(qui_widget* el) {
	for (s32 i = 0; i < el->children.length; i++) {
		qui_widget* w = *(qui_widget**)array_at(&el->children, i);
		log_assert(w->type == WIDGET_TOOLBAR_ITEM, "Toolbar can only have toolbar item widgets as children");
		_qui_close_entire_toolbar_item(w);
	}
}

qui_widget* qui_create_toolbar(qui_widget* qui)
{
	qui_widget* wg = _qui_create_empty_widget(qui);
	wg->width = 500;
	wg->height = TOOLBAR_H;
	wg->type = WIDGET_TOOLBAR;
	qui_widget* master_widget = _qui_find_parent_of_type(wg, WIDGET_MAIN);
	if (master_widget) array_push(&master_widget->special_children, (uint8_t*)&wg);
	else log_assert(0, "QUI needs a master element created by qui_setup()");
	return wg;
}

void _qui_render_toolbar(qui_widget* el) {
	renderer->render_rectangle(el->x, el->y, el->width, el->height, active_ui_style.widget_background_static);
	renderer->render_rectangle(el->x, el->y + el->height - 1, el->width, 1, active_ui_style.widget_border_outter_static);
}

void _qui_update_toolbar(qui_widget* el) {
	if (el->parent) {
		el->width = el->parent->width;
	}

	#define TOOLBAR_ITEM_OFFSETX (10)
	s32 offsetx = 0;
	for (s32 i = 0; i < el->children.length; i++) {
		qui_widget* w = *(qui_widget**)array_at(&el->children, i);
		w->x = TOOLBAR_ITEM_OFFSETX + el->x + offsetx;
		offsetx += w->width;
	}
}