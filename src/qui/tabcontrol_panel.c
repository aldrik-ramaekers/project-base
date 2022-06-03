void _qui_render_flex_container(qui_widget* el);
void _qui_update_flex_container(qui_widget* el);

static bool _qui_tabcontrol_panel_close_siblings(qui_widget* el) {
	qui_widget* button_bar = el->parent;
	for (s32 i = 0; i < button_bar->children.length; i++) {
		qui_widget* w = *(qui_widget**)array_at(&button_bar->children, i);
		tabcontrol_panel* data = (tabcontrol_panel*)w->data;
		if (w != el) {
			data->state = IDLE;
			data->container->visible = false;
		}

	}
	return false;
}

void _qui_update_tabcontrol_panel(qui_state* main_state, qui_widget* el) {
	tabcontrol_panel* data = (tabcontrol_panel*)el->data;

	s32 tw = renderer->calculate_text_width(global_ui_context.font_small, data->text);
	el->width = tw + (TABCONTROL_BUTTON_PADDING_W*2);
	el->height = el->parent->height; // Parent is fixed container button bar.

	vec4 actual_area = main_state->scissor_stack[main_state->scissor_index];
	if (_qui_mouse_interacts(main_state, actual_area)) {
		if (data->state == IDLE) data->state = HOVERED;
		if (is_left_clicked()) {
			main_state->window->do_draw = true;
			data->state = OPEN;
			data->container->visible = true;
			_qui_tabcontrol_panel_close_siblings(el);
		}
	}
	else {
		if (data->state == HOVERED) data->state = IDLE;
	}

	#define TABCONTROL_BUTTON_SELECTED_OFFSETY (3)
	el->y += TABCONTROL_BUTTON_SELECTED_OFFSETY;
	if (data->state == OPEN) el->y -= TABCONTROL_BUTTON_SELECTED_OFFSETY;
}

void _qui_render_tabcontrol_panel(qui_widget* el) {
	tabcontrol_panel* data = (tabcontrol_panel*)el->data;
	qui_widget_state state = data->state;

	color outter = active_ui_style.widget_border_outter_idle;
	color inner = active_ui_style.widget_border_inner_idle;
	color background = active_ui_style.widget_background_interactive_idle;

	if (state == HOVERED) {
		outter = active_ui_style.widget_border_outter_hovered;
		inner = active_ui_style.widget_border_inner_hovered;
		background = active_ui_style.widget_background_interactive_hovered;
	}
	else if (state == OPEN) {
		outter = active_ui_style.widget_border_outter_hovered;
		inner = active_ui_style.widget_border_inner_highlighted;
		background = active_ui_style.widget_background_interactive_highlighted;

	}

	#define EXTRA_HEIGHT_FOR_INTENTIONAL_OVERFLOW (10)
	el->height += EXTRA_HEIGHT_FOR_INTENTIONAL_OVERFLOW;

	renderer->render_rounded_rectangle(el->x,
		el->y,el->width, 
		el->height, 
		outter, 5.0f, 0);

	renderer->render_rounded_rectangle(el->x,
		el->y,
		el->width, 
		el->height,
		inner, 5.0f, 1);

	renderer->render_rounded_rectangle(el->x,
		el->y,
		el->width, 
		el->height,
		background, 5.0f, 2);

	if (data->text) {
		char* text = data->text;
		s32 tw = renderer->calculate_text_width(global_ui_context.font_small, text);
		renderer->render_text(global_ui_context.font_small, 
			el->x+(el->width/2)-(tw/2),
			el->y+(el->height/2)-(global_ui_context.font_small->px_h/2)-(EXTRA_HEIGHT_FOR_INTENTIONAL_OVERFLOW/2), 
			text, active_ui_style.widget_text);
	}
	el->height -= EXTRA_HEIGHT_FOR_INTENTIONAL_OVERFLOW;
}

qui_widget* qui_create_tabcontrol_panel(qui_widget* qui, char* title)
{
	log_assert(qui->type == WIDGET_TABCONTROL, "Tabcontrol panel can only be added to tabcontrol");

	qui_widget* layout = *(qui_widget**)array_at(&qui->children, 0);
	qui_widget* button_bar = *(qui_widget**)array_at(&layout->children, 1);
	button_bar = *(qui_widget**)array_at(&button_bar->children, 0);

	qui_widget* container = *(qui_widget**)array_at(&layout->children, 2);
	qui_widget* container_layout = *(qui_widget**)array_at(&container->children, 0);
	qui_widget* wg = _qui_create_empty_widget(button_bar);
	wg->type = WIDGET_TABCONTROL_PANEL;

	tabcontrol_panel* data = (tabcontrol_panel*)mem_alloc(sizeof(tabcontrol_panel));
	data->container = qui_create_flex_container(container_layout, 1);
	data->container->visible = false;
	data->state = IDLE;
	data->text = title;
	
	if (button_bar->children.length == 1) {
		data->state = OPEN;
		data->container->visible = true;
	}

	((qui_flex_container*)data->container->data)->color_background = &active_ui_style.widget_panel_background;
	((qui_flex_container*)data->container->data)->border = BORDER_TOP|BORDER_LEFT|BORDER_RIGHT|BORDER_BOTTOM;
	((qui_flex_container*)data->container->data)->border_size = 1;

	wg->data = (u8*)data;
	return data->container;
}