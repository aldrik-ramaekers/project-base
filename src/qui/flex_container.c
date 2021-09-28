void _qui_update_flex_container(qui_widget* el) {
}

void _qui_render_flex_container(qui_widget* el) {
	// Nothing to do.
}

qui_widget* qui_create_flex_container(qui_widget* qui, u8 flex)
{
	log_assert(qui->type == WIDGET_VERTICAL_LAYOUT || qui->type == WIDGET_HORIZONTAL_LAYOUT, "Flex container can only be added to vertical or horizontal layout");
	qui_widget* wg = _qui_create_empty_widget(qui);
	qui_flex_container* data = mem_alloc(sizeof(qui_flex_container));
	data->flex = flex;
	wg->data = (u8*)data;
	wg->type = WIDGET_FLEX_CONTAINER;
	return wg;
}