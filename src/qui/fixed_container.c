void _qui_update_fixed_container(qui_widget* el) {
}

void _qui_render_fixed_container(qui_widget* el) {
	renderer->render_rectangle(el->x, el->y, el->width, el->height, rgb(0,255,0));
}

qui_widget* qui_create_fixed_container(qui_widget* qui, u16 size)
{
	log_assert(qui->type == WIDGET_VERTICAL_LAYOUT, "Fixed container can only be added to vertical or horizontal layout");

	qui_widget* wg = _qui_create_empty_widget(qui);
	wg->type = WIDGET_FIXED_CONTAINER;
	if (qui && qui->type == WIDGET_VERTICAL_LAYOUT) {
		wg->height = size;
	}
	else {
		log_assert(0, "Fixed container must be placed in vertical or horizontal layout");
	}
	return wg;
}