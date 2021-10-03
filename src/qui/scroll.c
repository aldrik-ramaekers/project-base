
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

	scrollbar->height = scrollbar_height;
	scrollbar->x = scrollbar->parent->x;
	scrollbar->y = scrollbar->parent->y;

	s32 scrollable_px = -(content_size - container->height);
	s32 current_scroll = container->y - el->y;
	float scroll_percentage = current_scroll/(float)(-scrollable_px);

	s32 scrollable_px_for_scrollbar = (scrollbar_max_size - scrollbar->height);
	s32 scrollbar_offsety = scrollable_px_for_scrollbar * scroll_percentage;
	scrollbar->y -= scrollbar_offsety;

	#define MIN_SCROLLBAR_HEIGHT (6)
	if (scrollbar->height < MIN_SCROLLBAR_HEIGHT) scrollbar->height = MIN_SCROLLBAR_HEIGHT;

	vec4 actual_area = main_state->scissor_stack[main_state->scissor_index];
	if (_qui_mouse_interacts(main_state, actual_area)) {
		if (_global_mouse.scroll_state == SCROLL_UP) {
			container->scroll_y += SCROLL_SPEED_PX;
		}
		if (_global_mouse.scroll_state == SCROLL_DOWN) {
			container->scroll_y -= SCROLL_SPEED_PX;
		}
	}

	if (container->scroll_y > 0) container->scroll_y = 0;
	if (container->scroll_y < scrollable_px) container->scroll_y = scrollable_px;
}

void _qui_render_scroll(qui_widget* el) {
}

qui_widget* qui_create_vertical_scroll(qui_widget* qui)
{
	log_assert(qui->type == WIDGET_VERTICAL_LAYOUT || qui->type == WIDGET_HORIZONTAL_LAYOUT, "Scroll can only be added to vertical or horizontal layout");
	qui_widget* wg = qui_create_flex_container(qui, 1);
	qui_widget* scroll =  _qui_create_empty_widget(wg);
	scroll->type = WIDGET_SCROLL;
	qui_widget* layout = qui_create_horizontal_layout(scroll);
	
	qui_widget* container = qui_create_flex_container(layout, 1);
	qui_widget* scroll_box = qui_create_fixed_container(layout, SCROLLBAR_W);
	((qui_fixed_container*)scroll_box->data)->color_background = active_ui_style.scroll_background;
	((qui_fixed_container*)scroll_box->data)->border = BORDER_LEFT;
	((qui_fixed_container*)scroll_box->data)->border_size = 1;

	qui_widget* scrollbar_layout = qui_create_vertical_layout(scroll_box);
	_qui_create_scroll_button(scrollbar_layout, true);
	qui_widget* bar_container = qui_create_flex_container(scrollbar_layout, 1);
	_qui_create_scroll_button(scrollbar_layout, false);

	_qui_create_scroll_bar(bar_container);
	qui_widget* container_layout = qui_create_vertical_layout(container);
	return container_layout;
}