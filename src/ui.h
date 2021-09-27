/* 
*  BSD 2-Clause “Simplified” License
*  Copyright (c) 2019, Aldrik Ramaekers, aldrik.ramaekers@protonmail.com
*  All rights reserved.
*/

#ifndef INCLUDE_UI
#define INCLUDE_UI

#define TOOLBAR_ITEM_PADDING_W 10
#define TOOLBAR_ITEM_PADDING_OPTION_H 6

#define INTERACTIVE_ELEMENT_MARGIN_W 3
#define INTERACTIVE_ELEMENT_MARGIN_H 3

#define DRAG_BAR_SIZE 7
#define TOOLBAR_ITEM_OPTION_W 300
#define BUTTON_PADDING_W 30
#define BUTTON_PADDING_H 8

#define TOOLBAR_H 22

typedef struct t_qui_style
{
	color widget_text;
	color clear_color;
	color collapse_color;

	color widget_background_static;
	color widget_background_interactive_idle;
	color widget_background_interactive_highlighted;
	color widget_background_interactive_hovered;
	color widget_background_interactive_selected_option;

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
	WIDGET_MAIN,

	// Interactive elements.
	WIDGET_BUTTON,
	WIDGET_TOOLBAR,
	WIDGET_TOOLBAR_ITEM,
	WIDGET_TOOLBAR_ITEM_OPTION,

	// Layout elements.
	WIDGET_VERTICAL_LAYOUT,
	WIDGET_FIXED_CONTAINER,
	WIDGET_SIZE_CONTAINER,
	WIDGET_FLEX_CONTAINER,
} qui_widget_type;

typedef enum t_qui_widget_state 
{
	IDLE,
	DOWN,
	HOVERED,
	OPEN,
} qui_widget_state;

typedef struct t_qui_size_container
{
	enum {
		TOP,
		RIGHT,
		BOTTOM,
		LEFT,
	} direction;
} qui_size_container;

typedef struct t_qui_flex_container
{
	u8 flex;
} qui_flex_container;

typedef struct t_qui_button 
{
	char* text;
	qui_widget_state state;
} qui_button;

typedef struct t_qui_toolbar_item 
{
	char* text;
	qui_widget_state state;
} qui_toolbar_item;

typedef struct t_qui_widget 
{
	array children;
	array special_children; // Only used by master widget.
	struct t_qui_widget* parent;
	u16 width;
	u16 height;
	s16 x;
	s16 y;
	u8 margin_x;
	u8 margin_y;
	qui_widget_type type;
	u8* data; // Widget specific data.
} qui_widget;

qui_style active_ui_style;

qui_widget* qui_setup();
qui_widget* qui_create_button(qui_widget* qui, char* text);
qui_widget* qui_create_toolbar(qui_widget* qui);
qui_widget* qui_create_toolbar_item(qui_widget* toolbar, char* text);
qui_widget* qui_create_toolbar_item_option(qui_widget* qui, char* text);
qui_widget* qui_create_vertical_layout(qui_widget* qui); // Vertical layout always fills their entire parent!
qui_widget* qui_create_fixed_container(qui_widget* qui, u16 size); // When placed in vertical layout, size = height, else size = width
qui_widget* qui_create_size_container(qui_widget* qui, u8 dir, u16 start_size);
qui_widget* qui_create_flex_container(qui_widget* qui, u8 flex);

void qui_render(platform_window* window, qui_widget* qui);
void qui_update(platform_window* window, qui_widget* qui);

#endif