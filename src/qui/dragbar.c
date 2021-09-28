
qui_widget* qui_create_dragbar(qui_widget* qui)
{
	log_assert(qui->type == WIDGET_VERTICAL_LAYOUT || qui->type == WIDGET_HORIZONTAL_LAYOUT, "Dragbar can only be added to vertical or horizontal layout");
	qui_widget* wg = _qui_create_empty_widget(qui);
	wg->height = DRAG_BAR_SIZE;
	wg->type = WIDGET_DRAGBAR;
	return wg;
}

void _qui_render_dragbar(qui_widget* el) {
	// It is assumed to only be a top bar right now.
	renderer->render_rectangle(el->x, el->y, el->width, DRAG_BAR_SIZE, active_ui_style.widget_resize_bar_background);
	renderer->render_rectangle(el->x, el->y, el->width, 1, active_ui_style.widget_border_outter_static);
}

void _qui_update_dragbar(qui_widget* el) {
	
}