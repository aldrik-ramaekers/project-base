void _qui_update_flex_container(qui_state* main_state, qui_widget* el) {
	
}

void _qui_render_flex_container(qui_state* main_state, qui_widget* el) {
	qui_flex_container* data = (qui_flex_container*)el->data;
	if (data->color_background) {
		renderer->render_rectangle(el->x, el->y, el->width, el->height, *data->color_background);
	}
	_qui_render_container_borders(el, data->border, data->border_size);
}

qui_widget* qui_create_flex_container_s(qui_widget* qui, u8 flex, s32 min_size_px)
{
	if (qui) {
		log_assert(qui->type == WIDGET_VERTICAL_LAYOUT || qui->type == WIDGET_HORIZONTAL_LAYOUT, "Flex container can only be added to vertical or horizontal layout");
	}
	qui_widget* wg = _qui_create_empty_widget(qui);
	qui_flex_container* data = mem_alloc(sizeof(qui_flex_container));
	data->flex = flex;
	data->border_size = 1;
	data->min_size_px = min_size_px;
	data->border = BORDER_NONE;
	data->color_background = 0;
	wg->data = (u8*)data;
	wg->type = WIDGET_FLEX_CONTAINER;
	return wg;
}

qui_widget* qui_create_flex_container(qui_widget* qui, u8 flex)
{
	if (qui) {
		log_assert(qui->type == WIDGET_VERTICAL_LAYOUT || qui->type == WIDGET_HORIZONTAL_LAYOUT, "Flex container can only be added to vertical or horizontal layout");
	}
	qui_widget* wg = _qui_create_empty_widget(qui);
	qui_flex_container* data = mem_alloc(sizeof(qui_flex_container));
	data->flex = flex;
	data->border_size = 1;
	data->min_size_px = MINIMUM_FLEX_SIZE;
	data->border = BORDER_NONE;
	data->color_background = 0;
	wg->data = (u8*)data;
	wg->type = WIDGET_FLEX_CONTAINER;
	return wg;
}

void qui_flex_container_set_border(qui_widget* el, qui_border border, u8 border_size) {
	((qui_flex_container*)el->data)->border = border;
	((qui_flex_container*)el->data)->border_size = border_size;
}