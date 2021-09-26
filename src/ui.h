/* 
*  BSD 2-Clause “Simplified” License
*  Copyright (c) 2019, Aldrik Ramaekers, aldrik.ramaekers@protonmail.com
*  All rights reserved.
*/

#ifndef INCLUDE_UI
#define INCLUDE_UI

#define BUTTON_PADDING_W 30
#define BUTTON_PADDING_H 8

#define TOOLBAR_H 22

typedef struct t_qui_style
{
	color widget_text;
	color clear_color;

	color widget_background_static;
	color widget_background_interactive_idle;
	color widget_background_interactive_highlighted;
	color widget_background_interactive_hovered;

	color widget_border_outter_static;
	color widget_border_outter_idle;
	color widget_border_outter_highlighted;
	color widget_border_outter_hovered;

	color widget_border_inner_idle;
	color widget_border_inner_highlighted;
	color widget_border_inner_hovered;
} qui_style;

typedef enum t_qui_widget_type 
{
	WIDGET_BUTTON,
	WIDGET_TOOLBAR,
	WIDGET_TOOLBAR_ITEM,
} qui_widget_type;

typedef enum t_qui_widget_state {
	IDLE,
	DOWN,
	HOVERED,
	OPEN,
} qui_widget_state;

typedef struct t_qui_button {
	char* text;
	qui_widget_state state;
} qui_button;

typedef struct t_qui_toolbar_button {
	char* text;
	qui_widget_state state;
} qui_toolbar_button;

typedef struct t_qui_widget 
{
	array children;
	u16 width;
	u16 height;
	s16 x;
	s16 y;
	qui_widget_type type;
	u8* data; // Widget specific data.
} qui_widget;

typedef struct t_qui {
	array children;
} qui;

qui_style active_ui_style;

qui qui_setup();
qui_widget* qui_create_button(qui* qui, char* text);
qui_widget* qui_create_toolbar(qui* qui);
qui_widget* qui_create_toolbar_item(qui* qui, qui_widget* toolbar, char* text);

void qui_render(platform_window* window, qui* qui);
void qui_update(platform_window* window, qui* qui);

#endif