void _qui_update_scroll(qui_state* main_state, qui_widget* el) {
	_qui_fill_parent(el);

	qui_widget* layout = *(qui_widget**)array_at(&el->children, 0);
	qui_widget* container = *(qui_widget**)array_at(&layout->children, 0);
	qui_widget* scrollbox = *(qui_widget**)array_at(&layout->children, 1);
	qui_widget* scrollbox_content = *(qui_widget**)array_at(&scrollbox->children, 0);
	qui_widget* scrollbox_bar_container = *(qui_widget**)array_at(&scrollbox_content->children, 1);
	qui_widget* scrollbar = *(qui_widget**)array_at(&scrollbox_bar_container->children, 0);

	qui_widget* container_layout = *(qui_widget**)array_at(&container->children, 0);

	layout_widget* data = (layout_widget*)container_layout->data;

	s32 content_size = data->size;
	float fill_percentage = container->height/(float)content_size;
	s32 scrollbar_max_size = container->height-(SCROLLBAR_W*2);
	s32 scrollbar_height = scrollbar_max_size*fill_percentage;

	s32 scrollable_px = -(content_size - container->height);

	#define MIN_SCROLLBAR_HEIGHT (6)
	if (scrollbar->height < MIN_SCROLLBAR_HEIGHT) scrollbar->height = MIN_SCROLLBAR_HEIGHT;

	bool is_dragging = main_state->dragging_widget == el;
	vec4 actual_area = main_state->scissor_stack[main_state->scissor_index];

	// If mouse is within renderable area (parent), handle scrolling by scroll and mouse.
	if ((_qui_mouse_interacts_peak(main_state, actual_area) || is_dragging) && _qui_can_take_scroll(main_state, el)) {

		if (_global_mouse.scroll_state == SCROLL_UP) {
			container->scroll_y += SCROLL_SPEED_PX;
			main_state->window->do_draw = true;
		}
		if (_global_mouse.scroll_state == SCROLL_DOWN) {
			container->scroll_y -= SCROLL_SPEED_PX;
			main_state->window->do_draw = true;
		}

		if ((_global_mouse.x < actual_area.x+actual_area.w && _global_mouse.x > actual_area.x+actual_area.w - SCROLLBAR_W) || is_dragging)
		{
			if (is_left_down()) {
				platform_set_cursor(main_state->window, CURSOR_DRAG_VERTICAL);
				main_state->window->do_draw = true;

				main_state->dragging_widget = el;
				int height_of_possible_mouse_drag = scrollbar_max_size - scrollbar_height;
				int mouse_drag_start_y = scrollbar->parent->y;
				float percentage = ((_global_mouse.y - mouse_drag_start_y) - scrollbar_height/2) / (float)height_of_possible_mouse_drag;
				container->scroll_y = scrollable_px * percentage;
			}
			else {
				main_state->dragging_widget = 0;
			}
		}
	}

	// Set scrollbar position.
	{
		s32 current_scroll = container->y - el->y;
		float scroll_percentage = current_scroll/(float)(-scrollable_px);

		scrollbar->height = scrollbar_height;
		scrollbar->x = scrollbar->parent->x;
		scrollbar->y = scrollbar->parent->y;

		s32 scrollable_px_for_scrollbar = (scrollbar_max_size - scrollbar->height);
		s32 scrollbar_offsety = scrollable_px_for_scrollbar * scroll_percentage;
		scrollbar->y -= scrollbar_offsety;
	}

	// Keep scroll within bounds.
	if (container->scroll_y > 0) container->scroll_y = 0;
	if (container->scroll_y < scrollable_px) container->scroll_y = scrollable_px;
}

void _qui_render_scroll(qui_state* main_state, qui_widget* el) {
}

qui_widget* qui_create_vertical_scroll(qui_widget* qui)
{
	/*
	- Flex
		- Scroll
			- Horizontal Layout
				- Flex Container
					- Vertical layout (where scrollable content lives)
				- Scroll box
					- Vertical Layout
						- Button up
						- Bar container
							- Scroll bar
						- Button down
	*/

	log_assert(qui->type == WIDGET_VERTICAL_LAYOUT || qui->type == WIDGET_HORIZONTAL_LAYOUT, "Scroll can only be added to vertical or horizontal layout");
	qui_widget* wg = qui_create_flex_container(qui, 1);
	qui_widget* scroll =  _qui_create_empty_widget(wg);
	scroll->type = WIDGET_SCROLL;

	//qui_scroll_widget* data = mem_alloc(sizeof(qui_scroll_widget));
	//scroll->data = (u8*)data;
	
	qui_widget* layout = qui_create_horizontal_layout(scroll);
	
	qui_widget* container = qui_create_flex_container(layout, 1);
	qui_widget* scroll_box = qui_create_fixed_container(layout, SCROLLBAR_W);
	((qui_fixed_container*)scroll_box->data)->color_background = &active_ui_style.scroll_background;
	((qui_fixed_container*)scroll_box->data)->border = BORDER_LEFT|BORDER_RIGHT;
	((qui_fixed_container*)scroll_box->data)->border_size = 1;

	qui_widget* scrollbar_layout = qui_create_vertical_layout(scroll_box);
	_qui_create_scroll_button(scrollbar_layout, true);
	qui_widget* bar_container = qui_create_flex_container(scrollbar_layout, 1);
	_qui_create_scroll_button(scrollbar_layout, false);

	_qui_create_scroll_bar(bar_container);
	qui_widget* container_layout = qui_create_vertical_layout(container);
	return container_layout;
}