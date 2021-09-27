void _qui_update_size_container(qui_widget* el) {
}

void _qui_render_size_container(qui_widget* el) {
	qui_size_container* data = (qui_size_container*)el->data;
	renderer->render_rectangle(el->x, el->y, el->width, el->height, active_ui_style.widget_panel_background);
	if (data->direction == TOP) {
		renderer->render_rectangle(el->x, el->y, el->width, DRAG_BAR_SIZE, active_ui_style.widget_resize_bar_background);
		renderer->render_rectangle(el->x, el->y, el->width, 1, active_ui_style.widget_border_outter_static);
	}
}

qui_widget* qui_create_size_container(qui_widget* qui, u8 dir, u16 start_size)
{	
	log_assert(qui->type == WIDGET_VERTICAL_LAYOUT, "Size container can only be added to vertical or horizontal layout");
	log_assert(dir == TOP, "Only TOP direction is supported");
	qui_widget* wg = _qui_create_empty_widget(qui);
	qui_size_container* data = mem_alloc(sizeof(qui_size_container));
	data->direction = dir;
	wg->data = (u8*)data;
	wg->type = WIDGET_SIZE_CONTAINER;
	if (qui && qui->type == WIDGET_VERTICAL_LAYOUT) {
		wg->height = start_size;
	}
	else {
		log_assert(0, "Sized container must be placed in vertical or horizontal layout");
	}
	return wg;
}