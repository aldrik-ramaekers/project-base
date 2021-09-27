void _qui_update_vertical_layout(qui_widget* el) {
	if (el->parent) {
		el->x = el->parent->x;
		el->y = el->parent->y;
		el->width = el->parent->width;
		el->height = el->parent->height;
	}

	// Calculate size for flex elements.
	s32 fixed_height = 0;
	s32 flex_size = 0;
	for (s32 i = 0; i < el->children.length; i++) {
		qui_widget* w = *(qui_widget**)array_at(&el->children, i);
		if (w->type != WIDGET_FLEX_CONTAINER) {
			fixed_height += w->height;
			fixed_height += w->margin_y*2;
		}
		else {
			qui_flex_container* data = (qui_flex_container*)w->data;
			flex_size += data->flex;
		}
	}

	// Resize flex elements here.
	if (flex_size) {
		s32 height_remaining_for_flex_containers = el->height - fixed_height;
		s32 height_per_flex = height_remaining_for_flex_containers / flex_size;
		s32 rogue_pixels = height_remaining_for_flex_containers - (height_per_flex*flex_size);
		for (s32 i = 0; i < el->children.length; i++) {
			qui_widget* w = *(qui_widget**)array_at(&el->children, i);
			if (w->type == WIDGET_FLEX_CONTAINER) {
				qui_flex_container* data = (qui_flex_container*)w->data;
				w->height = (height_per_flex*data->flex);
				if (i == 0) w->height += rogue_pixels;
			}
		}
	}

	// Set position of elements.
	s32 offsety = 0;
	for (s32 i = 0; i < el->children.length; i++) {
		qui_widget* w = *(qui_widget**)array_at(&el->children, i);
		w->x = el->x + w->margin_x;
		w->y = el->y + offsety + w->margin_y;
		offsety += w->height + w->margin_y*2;
	}
}


void _qui_render_vertical_layout(qui_widget* el) {
	//renderer->render_rectangle(el->x, el->y, el->width, el->height, rgb(255,0,0));
}

qui_widget* qui_create_vertical_layout(qui_widget* qui)
{
	log_assert(qui, "Vertical layout must have a parent widget");
	qui_widget* wg = _qui_create_empty_widget(qui);
	wg->type = WIDGET_VERTICAL_LAYOUT;
	return wg;
}