static void _qui_size_container_set_bounds(qui_widget* el) {
	qui_size_container* data = (qui_size_container*)el->data;

	layout_widget* layout = (layout_widget*)el->parent->data;

	s32 size_left = 0;
	if (el->parent->type == WIDGET_VERTICAL_LAYOUT) {
		size_left = layout->size_left_for_flex + el->height;
	}
	else {
		size_left = layout->size_left_for_flex + el->width;
	}

	data->max = size_left;
	data->min = DRAG_BAR_SIZE + MINIMUM_SIZE_CONTAINER_SIZE;
}

void _qui_resize_size_container(qui_state* main_state, qui_widget* el) {
	
}

void _qui_update_size_container(qui_state* main_state, qui_widget* el) {
	_qui_size_container_set_bounds(el);
	qui_size_container* data = (qui_size_container*)el->data;
	
	bool is_dragging = main_state->dragging_widget == el;
	if (!is_left_down_peak() && is_dragging) {
		data->drag_start_size = -1;
		data->mouse_drag_start_pos = -1;
		main_state->dragging_widget = 0;
	}

	vec4 actual_area = main_state->scissor_stack[main_state->scissor_index];
	if (_qui_mouse_interacts(main_state, actual_area) && _qui_can_take_scroll(main_state, el)) {
		if (is_left_down()) {
			main_state->dragging_widget = el;
			if (data->drag_start_size == -1) {
				data->drag_start_size = el->height;
				data->mouse_drag_start_pos = _global_mouse.y;
			}
		}
	}

	if (data->drag_start_size != -1) {
		s32 diff = _global_mouse.y - data->mouse_drag_start_pos;
		el->height = data->drag_start_size - diff;	
	}
	if (el->height < data->min) el->height = data->min;
	if (el->height > data->max) el->height = data->max;
}

void _qui_render_size_container(qui_widget* el) {
	qui_size_container* data = (qui_size_container*)el->data;
	renderer->render_rectangle(el->x, el->y, el->width, el->height, data->color_background);
	_qui_render_container_borders(el, data->border, data->border_size);
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
	data->color_background = rgba(0,0,0,0);
	data->border = BORDER_NONE;
	data->border_size = 0;
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