void _qui_update_flex_container(qui_widget* el) {
	if (el->parent->type == WIDGET_VERTICAL_LAYOUT) {
		el->width = el->parent->width;
	}
}

void _qui_render_flex_container(qui_widget* el) {
	// Nothing to do.
}

qui_widget* qui_create_flex_container(qui_widget* qui, u8 flex)
{
	qui_widget* wg = _qui_create_empty_widget(qui);
	qui_flex_container* data = mem_alloc(sizeof(qui_flex_container));
	data->flex = flex;
	wg->data = (u8*)data;
	wg->type = WIDGET_FLEX_CONTAINER;
	if (qui && qui->type == WIDGET_VERTICAL_LAYOUT) {
		wg->height = flex;
	}
	else {
		log_assert(0, "Fixed container must be placed in vertical or horizontal layout");
	}
	return wg;
}