void _qui_update_fixed_container(qui_widget* el) {
}

void _qui_render_container_borders(qui_widget* el, qui_border border, u8 border_size) {
	if (border == BORDER_NONE) return;
	if (border & BORDER_BOTTOM) {
		renderer->render_rectangle(el->x, el->y + el->height-border_size, el->width, border_size, active_ui_style.widget_border_outter_static);
	}
	if (border & BORDER_RIGHT) {
		renderer->render_rectangle(el->x + el->width-border_size, el->y, border_size, el->height, active_ui_style.widget_border_outter_static);
	}
	if (border & BORDER_LEFT) {
		renderer->render_rectangle(el->x, el->y, border_size, el->height, active_ui_style.widget_border_outter_static);
	}
	if (border & BORDER_TOP) {
		log_assert(0, "Not implemented");
	}
}

void _qui_render_fixed_container(qui_widget* el) {
	qui_fixed_container* data = (qui_fixed_container*)el->data;
	_qui_render_container_borders(el, data->border, data->border_size);
}

qui_widget* qui_create_fixed_container(qui_widget* qui, u16 size)
{
	log_assert(qui->type == WIDGET_VERTICAL_LAYOUT || qui->type == WIDGET_HORIZONTAL_LAYOUT, "Fixed container can only be added to vertical or horizontal layout");

	qui_widget* wg = _qui_create_empty_widget(qui);
	wg->type = WIDGET_FIXED_CONTAINER;

	qui_fixed_container* data = mem_alloc(sizeof(qui_fixed_container));
	data->border = BORDER_NONE;
	data->border_size = 1;
	wg->data = (u8*)data;

	if (qui && qui->type == WIDGET_VERTICAL_LAYOUT) {
		wg->height = size;
	}
	else if (qui && qui->type == WIDGET_HORIZONTAL_LAYOUT) {
		wg->width = size;
	}
	return wg;
}