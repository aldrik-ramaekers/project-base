void _qui_update_vertical_layout(qui_widget* el) {
	if (el->parent) {
		_qui_fill_parent(el);
	}

	layout_widget* data = (layout_widget*)el->data;

	// Calculate size for flex elements.
	s32 fixed_height = 0;
	s32 reserved_height = 0;
	s32 flex_size = 0;
	for (s32 i = 0; i < el->children.length; i++) {
		qui_widget* w = *(qui_widget**)array_at(&el->children, i);
		if (w->type != WIDGET_FLEX_CONTAINER) {
			fixed_height += w->height;
			fixed_height += w->margin_y*2;
		}
		else {
			qui_flex_container* data = (qui_flex_container*)w->data;
			if (w->visible) {
				flex_size += data->flex;
				reserved_height += data->min_size_px;
			}
		}
	}

	// Resize flex elements here.
	s32 height_remaining_for_flex_containers = el->height - fixed_height;
	data->size_left_for_flex = height_remaining_for_flex_containers - reserved_height;
	if (flex_size) {
		s32 height_per_flex = height_remaining_for_flex_containers / flex_size;
		s32 rogue_pixels = height_remaining_for_flex_containers - (height_per_flex*flex_size);
		for (s32 i = 0; i < el->children.length; i++) {
			qui_widget* w = *(qui_widget**)array_at(&el->children, i);
			if (w->type == WIDGET_FLEX_CONTAINER) {
				qui_flex_container* data = (qui_flex_container*)w->data;
				if (!w->visible) {
					w->height = 0;
					continue;
				}
				w->height = (height_per_flex*data->flex) + rogue_pixels - (w->margin_y*2);
				rogue_pixels = 0;
			}
		}
	}

	// Set position of elements.
	s32 offsety = 0;
	for (s32 i = 0; i < el->children.length; i++) {
		qui_widget* w = *(qui_widget**)array_at(&el->children, i);
		w->x = el->x + w->margin_x;
		w->y = el->y + offsety + w->margin_y + w->scroll_y;
		w->width = el->width - w->margin_x*2;
		offsety += w->height + w->margin_y*2;
	}
	data->fixed_size = fixed_height + reserved_height;

	//el->height = el->parent->height;
	if (el->height < fixed_height) el->height = fixed_height;
	if (el->height > el->parent->height) el->height = el->parent->height;
	
	data->size = el->height;
}


void _qui_render_vertical_layout(qui_widget* el) {
	//renderer->render_rectangle(el->x, el->y, el->width, el->height, rgb(255,0,0));
}

qui_widget* qui_create_vertical_layout(qui_widget* qui)
{
	log_assert(qui, "Vertical layout must have a parent widget");
	log_assert(qui->type == WIDGET_FIXED_CONTAINER || 
		qui->type == WIDGET_FLEX_CONTAINER || 
		qui->type == WIDGET_SIZE_CONTAINER || 
		qui->type == WIDGET_MAIN || 
		qui->type == WIDGET_TABCONTROL ||
		qui->type == WIDGET_SCROLL, 
		"Vertical layout can only be added to container, main widget, scroll or tabcontrol");
	qui_widget* wg = _qui_create_empty_widget(qui);
	wg->type = WIDGET_VERTICAL_LAYOUT;
	layout_widget* data = mem_alloc(sizeof(layout_widget));
	data->fixed_size = 0;
	data->size_left_for_flex = 0;
	wg->data = (u8*)data;
	return wg;
}