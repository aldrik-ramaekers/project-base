void _qui_update_fixed_container(qui_widget* el) {
}

void _qui_render_fixed_container(qui_widget* el) {
	qui_fixed_container* data = (qui_fixed_container*)el->data;
	if (data->border == BORDER_NONE) return;
	if (data->border & BORDER_BOTTOM) {
		renderer->render_rectangle(el->x, el->y + el->height-1, el->width, 1, active_ui_style.widget_border_outter_static);
	}
	else {
		log_assert(0, "Not implemented");
	}
}

qui_widget* qui_create_fixed_container(qui_widget* qui, u16 size)
{
	log_assert(qui->type == WIDGET_VERTICAL_LAYOUT || qui->type == WIDGET_HORIZONTAL_LAYOUT, "Fixed container can only be added to vertical or horizontal layout");

	qui_widget* wg = _qui_create_empty_widget(qui);
	wg->type = WIDGET_FIXED_CONTAINER;

	qui_fixed_container* data = mem_alloc(sizeof(qui_fixed_container));
	data->border = BORDER_NONE;
	wg->data = (u8*)data;

	if (qui && qui->type == WIDGET_VERTICAL_LAYOUT) {
		wg->height = size;
	}
	else if (qui && qui->type == WIDGET_HORIZONTAL_LAYOUT) {
		wg->width = size;
	}
	return wg;
}