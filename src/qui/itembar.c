qui_widget* qui_create_itembar(qui_widget* qui)
{
	log_assert(qui->type == WIDGET_VERTICAL_LAYOUT || qui->type == WIDGET_HORIZONTAL_LAYOUT, "Fixed container can only be added to vertical or horizontal layout");

	qui_widget* wg = qui_create_fixed_container(qui, ITEMBAR_H);
	((qui_fixed_container*)(wg->data))->border |= BORDER_BOTTOM;

	qui_widget* layout = 0;
	if (qui && qui->type == WIDGET_VERTICAL_LAYOUT) {
		layout = qui_create_horizontal_layout(wg);
	}
	else if (qui && qui->type == WIDGET_HORIZONTAL_LAYOUT) {
		layout = qui_create_vertical_layout(wg);
	}
	qui_create_fixed_container(layout, TOOLBAR_ITEM_PADDING_W);
	return layout;
}

qui_widget* qui_create_itembar_separator(qui_widget* qui) {
	qui_widget* wg = qui_create_fixed_container(qui, 1);
	((qui_fixed_container*)(wg->data))->border |= BORDER_RIGHT;
	wg->margin_y = 5;
	wg->margin_x = 5;
	return wg;
}