qui_widget* qui_create_table_row_entry(qui_widget* qui, char* text) {
	qui_widget* wg = qui_create_flex_container(qui, 1);
	qui_flex_container_set_border(wg, BORDER_LEFT, 1);
	qui_widget* layout = qui_create_horizontal_layout(wg);
	qui_widget* label = qui_create_label(layout, text, false);
	label->margin_x = 5;
	return layout;
}

//color table_row_color = rgba(0,0,0,100); // TODO move this to style struct
qui_widget* qui_create_table_row_header(qui_widget* qui) {
	qui_widget* wg = qui_create_fixed_container(qui, TABLE_ROW_H);
	qui_fixed_container_set_border(wg, BORDER_BOTTOM, 1);
	((qui_fixed_container*)wg->data)->color_background = 0;//&table_row_color;
	qui_widget* layout = qui_create_horizontal_layout(wg);
	return layout;
}

qui_widget* qui_create_table_row(qui_widget* qui) {
	qui_widget* wg = qui_create_fixed_container(qui, TABLE_ROW_H);
	qui_fixed_container_set_border(wg, BORDER_BOTTOM, 1);
	qui_widget* layout = qui_create_horizontal_layout(wg);
	return layout;
}

qui_widget* qui_create_table(qui_widget* qui) {
	log_assert(qui->type == WIDGET_VERTICAL_LAYOUT || qui->type == WIDGET_HORIZONTAL_LAYOUT, "Table can only be added to vertical or horizontal layout");
	qui_widget* wg = qui_create_flex_container(qui, 1);
	qui_flex_container_set_border(wg, BORDER_TOP|BORDER_RIGHT|BORDER_BOTTOM|BORDER_LEFT, 1);
	qui_widget* layout = qui_create_vertical_layout(wg);
	qui_widget* scroll = qui_create_vertical_scroll(layout);
	return scroll;
}