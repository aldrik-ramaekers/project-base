void _qui_update_tabcontrol_panel(qui_widget* el) {
	
}

void _qui_render_tabcontrol_panel(qui_widget* el) {
	
}

qui_widget* qui_create_tabcontrol_panel(qui_widget* qui, char* title)
{
	log_assert(qui->type == WIDGET_TABCONTROL, "Tabcontrol panel can only be added to tabcontrol");
	qui_widget* wg = _qui_create_empty_widget(qui);
	wg->type = WIDGET_TABCONTROL_PANEL;
	return wg;
}