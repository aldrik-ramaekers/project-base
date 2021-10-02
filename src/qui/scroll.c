void _qui_update_scroll_button(qui_widget* el) {
}

void _qui_render_scroll_button(qui_widget* el) {
}

void _qui_create_scroll_button(qui_widget* el) {

}

void _qui_update_scroll(qui_widget* el) {
	_qui_fill_parent(el);
}

void _qui_render_scroll(qui_widget* el) {
}

qui_widget* qui_create_vertical_scroll(qui_widget* qui)
{
	log_assert(qui->type == WIDGET_VERTICAL_LAYOUT || qui->type == WIDGET_HORIZONTAL_LAYOUT, "Size container can only be added to vertical or horizontal layout");
	qui_widget* wg = qui_create_flex_container(qui, 1);
	qui_widget* layout = qui_create_horizontal_layout(wg);
	
	qui_widget* container = qui_create_flex_container(layout, 1);
	qui_widget* scroll_box = qui_create_fixed_container(layout, SCROLLBAR_W);
	((qui_fixed_container*)scroll_box->data)->color_background = active_ui_style.scroll_background;
	((qui_fixed_container*)scroll_box->data)->border = BORDER_LEFT;
	((qui_fixed_container*)scroll_box->data)->border_size = 1;

	qui_widget* scrollbar_layout = qui_create_vertical_layout(scroll_box);

	return container;
}