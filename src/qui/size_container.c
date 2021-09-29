void _qui_update_size_container(qui_widget* el) {
}

void _qui_render_size_container(qui_widget* el) {
	
}

qui_widget* qui_create_size_container(qui_widget* qui, u8 dir, u16 start_size)
{	
	log_assert(qui->type == WIDGET_VERTICAL_LAYOUT || qui->type == WIDGET_HORIZONTAL_LAYOUT, "Size container can only be added to vertical or horizontal layout");
	log_assert(dir == DIRECTION_TOP, "Only TOP direction is supported");
	qui_widget* wg = _qui_create_empty_widget(qui);
	qui_size_container* data = mem_alloc(sizeof(qui_size_container));
	data->direction = dir;
	wg->data = (u8*)data;
	wg->type = WIDGET_SIZE_CONTAINER;
	if (qui && qui->type == WIDGET_VERTICAL_LAYOUT) {
		wg->height = start_size;
	}
	else if (qui && qui->type == WIDGET_HORIZONTAL_LAYOUT) {
		wg->width = start_size;
	}
	qui_widget* layout = qui_create_vertical_layout(wg);
	qui_create_dragbar(layout);
	qui_widget* flex_container = qui_create_flex_container(layout, 1);
	return flex_container;
}