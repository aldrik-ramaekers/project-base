qui_widget* qui_create_itembar(qui_widget* qui)
{
	log_assert(qui->type == WIDGET_VERTICAL_LAYOUT || qui->type == WIDGET_HORIZONTAL_LAYOUT, "Fixed container can only be added to vertical or horizontal layout");

	qui_widget* wg = qui_create_fixed_container(qui, ITEMBAR_H);
	((qui_fixed_container*)(wg->data))->border |= BORDER_BOTTOM;

	if (qui && qui->type == WIDGET_VERTICAL_LAYOUT) {
		qui_widget* layout = qui_create_horizontal_layout(wg);
		return layout;
	}
	else if (qui && qui->type == WIDGET_HORIZONTAL_LAYOUT) {
		qui_widget* layout = qui_create_vertical_layout(wg);
		return layout;
	}
	log_assert(0, "Unreachable");
	return wg;
}