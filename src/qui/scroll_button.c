
void _qui_update_scroll_button(qui_widget* el) {
}

void _qui_render_scroll_button(qui_widget* el) {
	color outter = active_ui_style.widget_border_outter_idle;
	color inner = active_ui_style.widget_border_inner_idle;
	color background = active_ui_style.widget_background_interactive_idle;

#if 0
	if (state == HOVERED) {
		outter = active_ui_style.widget_border_outter_hovered;
		inner = active_ui_style.widget_border_inner_hovered;
		background = active_ui_style.widget_background_interactive_hovered;
	}
	else if (state == DOWN) {
		outter = active_ui_style.widget_border_outter_highlighted;
		inner = active_ui_style.widget_border_inner_highlighted;
		background = active_ui_style.widget_background_interactive_highlighted;
	}
#endif

	renderer->render_rounded_rectangle(el->x,
		el->y,el->width, 
		el->height, 
		outter, 2.0f, 0);

	renderer->render_rounded_rectangle(el->x,
		el->y,
		el->width, 
		el->height,
		inner, 2.0f, 1);

	renderer->render_rounded_rectangle(el->x,
		el->y,
		el->width, 
		el->height,
		background, 2.0f, 2);
}

void _qui_create_scroll_button(qui_widget* qui) {
	log_assert(qui->type == WIDGET_VERTICAL_LAYOUT || qui->type == WIDGET_HORIZONTAL_LAYOUT, "Scroll button can only be added to vertical or horizontal layout");
	qui_widget* el = _qui_create_empty_widget(qui);
	el->type = WIDGET_SCROLL_BUTTON;
	el->width = SCROLLBAR_W;
	el->height = SCROLLBAR_W;
}