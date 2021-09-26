//////// General setup
qui qui_setup()
{
	active_ui_style.widget_text = rgb(249, 249, 249);
	active_ui_style.clear_color = rgb(38, 38, 38);

	active_ui_style.widget_background_static = rgb(31, 31, 31);
	active_ui_style.widget_background_interactive_idle = rgb(51, 51, 51);
	active_ui_style.widget_background_interactive_highlighted = rgb(59, 64, 71);
	active_ui_style.widget_background_interactive_hovered = rgb(61, 61, 61);

	active_ui_style.widget_border_inner_idle = rgb(72, 72, 72);
	active_ui_style.widget_border_inner_highlighted = rgb(80, 85, 90);
	active_ui_style.widget_border_inner_hovered = rgb(82, 82, 82);

	active_ui_style.widget_border_outter_static = rgb(24, 24, 24);
	active_ui_style.widget_border_outter_idle = rgb(27, 27, 27);
	active_ui_style.widget_border_outter_highlighted = rgb(80, 160, 240);
	active_ui_style.widget_border_outter_hovered = rgb(27, 27, 27);

	qui q;
	q.children = array_create(sizeof(qui_widget*));
	return q;
}

//////// Widget setup
qui_widget* qui_create_button(qui* qui, char* text)
{
	qui_widget* wg = mem_alloc(sizeof(qui_widget));
	qui_button* data = mem_alloc(sizeof(qui_button));
	data->text = text;
	data->state = IDLE;
	wg->children = array_create(sizeof(qui_widget));
	wg->data = (u8*)data;
	wg->width = 100;
	wg->type = WIDGET_BUTTON;
	wg->height = global_ui_context.font_small->px_h + (BUTTON_PADDING_H*2);
	wg->x = 50;
	wg->y = 50;
	array_push(&qui->children, (uint8_t*)&wg);
	return wg;
}

qui_widget* qui_create_toolbar(qui* qui)
{
	qui_widget* wg = mem_alloc(sizeof(qui_widget));
	wg->children = array_create(sizeof(qui_widget));
	wg->data = 0;
	wg->width = 500;
	wg->height = TOOLBAR_H;
	wg->type = WIDGET_TOOLBAR;
	wg->x = 0;
	wg->y = 0;
	array_push(&qui->children, (uint8_t*)&wg);
	return wg;
}

qui_widget* qui_create_toolbar_item(qui* qui, qui_widget* toolbar, char* text)
{
	qui_widget* wg = mem_alloc(sizeof(qui_widget));
	qui_toolbar_button* data = mem_alloc(sizeof(qui_toolbar_button));
	data->text = text;
	data->state = IDLE;
	wg->children = array_create(sizeof(qui_widget));
	wg->data = (u8*)data;
	wg->width = 100;
	wg->type = WIDGET_TOOLBAR_ITEM;
	wg->height = TOOLBAR_H-1;
	wg->x = 0;
	wg->y = 0;
	array_push(&toolbar->children, (uint8_t*)&wg);
	return wg;
}

//////// Widget render functions
static void _qui_render_toolbar(qui_widget* parent) {
	renderer->render_rectangle(parent->x, parent->y, parent->width, parent->height, active_ui_style.widget_background_static);
	renderer->render_rectangle(parent->x, parent->y + parent->height - 1, parent->width, 1, active_ui_style.widget_border_outter_static);
}

static void _qui_render_button(qui_widget* parent) {
	char* text = ((qui_button*)parent->data)->text;
	int state = ((qui_button*)parent->data)->state;
	s32 tw = renderer->calculate_text_width(global_ui_context.font_small, text);

	color outter = active_ui_style.widget_border_outter_idle;
	color inner = active_ui_style.widget_border_inner_idle;
	color background = active_ui_style.widget_background_interactive_idle;

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

	renderer->render_rounded_rectangle(parent->x,
		parent->y,parent->width, 
		parent->height, 
		outter, 5.0f, 0);
	
	renderer->render_rounded_rectangle(parent->x,
		parent->y,
		parent->width, 
		parent->height,
		inner, 5.0f, 1);

	renderer->render_rounded_rectangle(parent->x,
		parent->y,
		parent->width, 
		parent->height,
		background, 5.0f, 2);
		
	renderer->render_text(global_ui_context.font_small, 
		parent->x+(parent->width/2)-(tw/2),
		parent->y+(parent->height/2)-(global_ui_context.font_small->px_h/2), 
		text, active_ui_style.widget_text);
}

//////// Widget update functions
static void _qui_update_toolbar(qui_widget* parent) {
	// Nothing to do for toolbar.
}

static void _qui_update_button(qui_widget* parent) {
	parent->width = renderer->calculate_text_width(global_ui_context.font_small, (char*)parent->data) + (BUTTON_PADDING_W*2);
	if (mouse_interacts(parent->x, parent->y, parent->width, parent->height)) {
		((qui_button*)parent->data)->state = HOVERED;
		if (is_left_down()) {
			((qui_button*)parent->data)->state = DOWN;
		}
	}
	else {
		((qui_button*)parent->data)->state = IDLE;
	}
}

//////// General qui functions
void _qui_render_widget(qui_widget* parent) {
	if (parent->type == WIDGET_BUTTON) _qui_render_button(parent);
	if (parent->type == WIDGET_TOOLBAR) _qui_render_toolbar(parent);
}

void _qui_update_widget(qui_widget* parent) {
	if (parent->type == WIDGET_BUTTON) _qui_update_button(parent);
	if (parent->type == WIDGET_TOOLBAR) _qui_update_toolbar(parent);
}

void qui_render(platform_window* window, qui* parent) {
	renderer->render_clear(window, active_ui_style.clear_color);
	for (s32 i = 0; i < parent->children.length; i++) {
		qui_widget* w = *(qui_widget**)array_at(&parent->children, i);
		_qui_render_widget(w);
	}
}

void qui_update(platform_window* window, qui* parent) {
	for (s32 i = 0; i < parent->children.length; i++) {
		qui_widget* w = *(qui_widget**)array_at(&parent->children, i);
		_qui_update_widget(w);
	}
}