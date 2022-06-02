
qui_widget* qui_create_dragbar(qui_widget* qui)
{
	log_assert(qui->type == WIDGET_VERTICAL_LAYOUT || qui->type == WIDGET_HORIZONTAL_LAYOUT, "Dragbar can only be added to vertical or horizontal layout");
	qui_widget* wg = qui_create_fixed_container(qui, DRAG_BAR_SIZE);
	((qui_fixed_container*)wg->data)->color_background = &active_ui_style.widget_resize_bar_background;
	qui_fixed_container_set_border(wg, BORDER_TOP, 1);
	return wg;
}