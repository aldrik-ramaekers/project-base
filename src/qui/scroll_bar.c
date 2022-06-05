
void _qui_update_scroll_bar(qui_state* main_state, qui_widget* el) {
	el->x = el->parent->x;
}

void _qui_render_scroll_bar(qui_state* main_state, qui_widget* el) {
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

	renderer->render_rectangle(el->x,
		el->y,el->width, 
		el->height, 
		outter);

	renderer->render_rectangle(el->x+1,
		el->y+1,
		el->width-2, 
		el->height-2,
		inner);

	renderer->render_rectangle(el->x+2,
		el->y+2,
		el->width-4, 
		el->height-4,
		background);
}

void _qui_create_scroll_bar(qui_widget* qui) {
	qui_widget* el = _qui_create_empty_widget(qui);
	el->type = WIDGET_SCROLL_BAR;
	el->width = SCROLLBAR_W;
	el->height = SCROLLBAR_W;
}