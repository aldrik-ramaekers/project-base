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

	el->x = el->parent->x;
	el->y = el->parent->y;
	el->width = el->parent->width;
	el->height = el->parent->height;

	_qui_update_tabcontrol_buttons(el);
}

void _qui_render_tabcontrol(qui_widget* el) {
	renderer->render_rectangle(el->x, el->y, el->width, el->height, rgb(255,0,0));
}

qui_widget* qui_create_tabcontrol(qui_widget* qui)
{
	log_assert(qui->type == WIDGET_VERTICAL_LAYOUT || qui->type == WIDGET_HORIZONTAL_LAYOUT, "Tabcontrol can only be added to vertical or horizontal layout");
	qui_widget* wg = qui_create_flex_container(qui, 1);
	qui_widget* wg2 = _qui_create_empty_widget(wg);
	wg2->type = WIDGET_TABCONTROL;

	qui_widget* layout = qui_create_vertical_layout(wg2);
	qui_create_fixed_container(layout, 5);
	qui_create_fixed_container(layout, 30);
	qui_widget* container = qui_create_flex_container(layout, 1);
	((qui_flex_container*)container->data)->color_background = rgb(0,0,255);
	return wg2;
}