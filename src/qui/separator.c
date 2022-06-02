
qui_widget* qui_create_separator(qui_widget* qui, u16 size)
{
	qui_widget* wg = qui_create_fixed_container(qui, size);
	((qui_fixed_container*)wg->data)->color_background = &active_ui_style.widget_resize_bar_background;
	return wg;
}