
void _qui_update_size_container(qui_widget* el) {
	qui_widget* layout = *(qui_widget**)array_at(&el->children, 0);
	qui_widget* dragbar = *(qui_widget**)array_at(&layout->children, 0);

	qui_size_container* data = (qui_size_container*)el->data;
	
	if (!is_left_down_peak()) {
		data->drag_start_size = -1;
		data->mouse_drag_start_pos = -1;
	}

	if (mouse_interacts(dragbar->x, dragbar->y, dragbar->width, dragbar->height)) {
		if (is_left_down()) {
			if (data->drag_start_size == -1) {
				data->drag_start_size = el->height;
				data->mouse_drag_start_pos = _global_mouse.y;
			}
		}
	}

	if (data->drag_start_size != -1) {
		s32 diff = _global_mouse.y - data->mouse_drag_start_pos;
		el->height = data->drag_start_size - diff;

		// MOUSE_OFFSCREEN verneukt dit
		if (el->height < data->min) el->height = data->min;
		if (el->height > data->max) el->height = data->max;
	}
}

void _qui_render_size_container(qui_widget* el) {
	
}

void qui_set_size_container_bounds(qui_widget* el, u32 min, u32 max) {
	qui_widget* size_container = _qui_find_parent_of_type(el, WIDGET_SIZE_CONTAINER);
	log_assert(size_container, "Function can only be called on size container");
	qui_size_container* data = (qui_size_container*)size_container->data;
	data->max = max;
	data->min = min;
	if (data->min < DRAG_BAR_SIZE) data->min = DRAG_BAR_SIZE;
}

qui_widget* qui_create_size_container(qui_widget* qui, u8 dir, u16 start_size)
{	
	log_assert(qui->type == WIDGET_VERTICAL_LAYOUT || qui->type == WIDGET_HORIZONTAL_LAYOUT, "Size container can only be added to vertical or horizontal layout");
	log_assert(dir == DIRECTION_TOP, "Only TOP direction is supported");
	qui_widget* wg = _qui_create_empty_widget(qui);
	qui_size_container* data = mem_alloc(sizeof(qui_size_container));
	data->direction = dir;
	data->max = 9999;
	data->min = 0;
	data->mouse_drag_start_pos = -1;
	data->drag_start_size = -1;
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