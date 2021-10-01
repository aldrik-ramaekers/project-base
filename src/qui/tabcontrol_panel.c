void _qui_update_tabcontrol_panel(qui_widget* el) {
	tabcontrol_panel* data = (tabcontrol_panel*)el->data;
	s32 tw = renderer->calculate_text_width(global_ui_context.font_small, data->text);
	el->width = tw;
	el->height = el->parent->height; // Parent is fixed container button bar.
}

void _qui_render_tabcontrol_panel(qui_widget* el) {
	tabcontrol_panel* data = (tabcontrol_panel*)el->data;
	renderer->render_rectangle(el->x, el->y, el->width, el->height, rgb(0,255,0));
	renderer->render_text(global_ui_context.font_small, el->x, el->y, data->text, active_ui_style.widget_text);
}

qui_widget* qui_create_tabcontrol_panel(qui_widget* qui, char* title)
{
	log_assert(qui->type == WIDGET_TABCONTROL, "Tabcontrol panel can only be added to tabcontrol");

	qui_widget* layout = *(qui_widget**)array_at(&qui->children, 0);
	qui_widget* button_bar = *(qui_widget**)array_at(&layout->children, 0);
	qui_widget* wg = _qui_create_empty_widget(button_bar);
	wg->type = WIDGET_TABCONTROL_PANEL;

	tabcontrol_panel* data = (tabcontrol_panel*)mem_alloc(sizeof(tabcontrol_panel));
	data->container = 0;
	data->open = false;
	data->text = title;
	wg->data = (u8*)data;
	return wg;
}