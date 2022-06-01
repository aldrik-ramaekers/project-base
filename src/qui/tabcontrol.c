static void _qui_update_tabcontrol_buttons(qui_widget* el) {
	qui_widget* layout = *(qui_widget**)array_at(&el->children, 0);
	qui_widget* button_bar = *(qui_widget**)array_at(&layout->children, 1);

	#define TABCONTROL_BUTTON_START_OFFSET (5)
	#define TABCONTROL_BUTTON_SELECTED_OFFSETY (3)
	s32 offsetx = TABCONTROL_BUTTON_START_OFFSET;
	for (s32 i = 0; i < button_bar->children.length; i++) {
		qui_widget* w = *(qui_widget**)array_at(&button_bar->children, i);
		w->x = button_bar->x + offsetx;
		offsetx += w->width;
		w->y = button_bar->y+button_bar->height-w->height+TABCONTROL_BUTTON_SELECTED_OFFSETY;
		tabcontrol_panel* data = (tabcontrol_panel*)w->data;
		if (data->state == OPEN) w->y -= TABCONTROL_BUTTON_SELECTED_OFFSETY;
	}
}

void _qui_update_tabcontrol(qui_widget* el) {
	log_assert(el->parent, "Tabcontrol does not have a parent");
	log_assert(el->parent->type == WIDGET_FLEX_CONTAINER, "Tabcontrol parent must be a flex container");

	_qui_fill_parent(el);
	_qui_update_tabcontrol_buttons(el);
}

void _qui_render_tabcontrol(qui_widget* el) {
}

qui_widget* qui_create_tabcontrol(qui_widget* qui)
{
	/*

	- Flex
		- Tabcontrol (return value)
			- Vertical layout
				- Padding
				- Fixed container (buttons)
					- Horizontal Layout
				- Flex container (content)
					- Horizontal layout

	*/

	log_assert(qui->type == WIDGET_VERTICAL_LAYOUT || qui->type == WIDGET_HORIZONTAL_LAYOUT, "Tabcontrol can only be added to vertical or horizontal layout");
	qui_widget* wg = qui_create_flex_container(qui, 1);
	qui_widget* wg2 = _qui_create_empty_widget(wg);
	wg2->type = WIDGET_TABCONTROL;
	wg2->margin_x = 5;
	wg2->margin_y = 5;

	qui_widget* layout = qui_create_vertical_layout(wg2);
	qui_create_fixed_container(layout, 5); // Extra padding top.
	qui_widget* button_container = qui_create_fixed_container(layout, 30); // Container for the buttons.
	//qui_create_horizontal_layout(button_container);
	//((qui_fixed_container*)button_row->data)->border = BORDER_BOTTOM;

	qui_widget* container = qui_create_flex_container(layout, 1); // Flex container for tab content.
	qui_create_horizontal_layout(container); // Horizontal layout containing all tabs. (added in tabcontrol_panel.c)
	return wg2;
}