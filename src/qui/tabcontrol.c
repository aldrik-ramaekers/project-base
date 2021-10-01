void _qui_update_tabcontrol(qui_widget* el) {
	log_assert(el->parent, "Tabcontrol does not have a parent");
	log_assert(el->parent->type == WIDGET_FLEX_CONTAINER, "Tabcontrol parent must be a flex container");

	el->x = el->parent->x;
	el->y = el->parent->y;
	el->width = el->parent->width;
	el->height = el->parent->height;
}

void _qui_render_tabcontrol(qui_widget* el) {
	renderer->render_rectangle(el->x, el->y, el->width, el->height, rgb(255,0,0));
}

qui_widget* qui_create_tabcontrol(qui_widget* qui)
{
	log_assert(qui->type == WIDGET_VERTICAL_LAYOUT || qui->type == WIDGET_HORIZONTAL_LAYOUT, "Tabcontrol can only be added to vertical or horizontal layout");
	qui_widget* wg = qui_create_flex_container(qui, 1);
	qui_widget* wg2 = _qui_create_empty_widget(wg);
	wg2->type = WIDGET_TABCONTROL;
	return wg2;
}