/* 
*  BSD 2-Clause “Simplified” License
*  Copyright (c) 2022, Aldrik Ramaekers, aldrik.ramaekers@protonmail.com
*  All rights reserved.
*/

/*

The idea behind this gui library is as following:
- Widgets can only be placed within Horizontal and Vertical layout widgets.
- Positioning of widgets is handled by their parent layout widget.
- Layout widgets can only be added to Flex and Fixed containers.
- Flex and Fixed containers can only be added to layout widgets.
- The topmost widget is a fixed widget.
- Flex and Fixed containers can only have 1 child.

*/

#ifndef INCLUDE_UI
#define INCLUDE_UI

#define TOOLBAR_ITEM_PADDING_W 10
#define TOOLBAR_ITEM_PADDING_OPTION_H 6

#define INTERACTIVE_ELEMENT_MARGIN_W 3
#define INTERACTIVE_ELEMENT_MARGIN_H 3

#define INTERACTIVE_ICON_ELEMENT_MARGIN_S (2)

#define DRAG_BAR_SIZE 8
#define TOOLBAR_ITEM_OPTION_W 300
#define BUTTON_PADDING_W 30
#define BUTTON_PADDING_H 8

#define TABCONTROL_BUTTON_PADDING_W 5

#define LABEL_PADDING_W 0
#define MINIMUM_FLEX_SIZE (100)
#define MINIMUM_SIZE_CONTAINER_SIZE (30)

#define ITEMBAR_H 30
#define TOOLBAR_H 22

#define SCROLL_SPEED_PX (15)
#define SCROLLBAR_W (15)

#define TABLE_ROW_H (25)

typedef struct t_qui_style
{
	color widget_text;
	color clear_color;
	color collapse_color;
	color scroll_background;

	color widget_resize_bar_background;
	color widget_panel_background;

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
	WIDGET_LABEL,
	WIDGET_DROPDOWN,
	WIDGET_DROPDOWN_OPTION,
	WIDGET_TABCONTROL,
	WIDGET_TABCONTROL_PANEL,
	WIDGET_SCROLL,
	WIDGET_SCROLL_BUTTON,
	WIDGET_SCROLL_BAR,

	// Layout elements.
	WIDGET_VERTICAL_LAYOUT,
	WIDGET_HORIZONTAL_LAYOUT,
	WIDGET_FIXED_CONTAINER,
	WIDGET_SIZE_CONTAINER,
	WIDGET_FLEX_CONTAINER,
} qui_widget_type;

typedef enum t_qui_border {
	BORDER_NONE = 0,
	BORDER_TOP = 1,
	BORDER_RIGHT = 2,
	BORDER_BOTTOM = 4,
	BORDER_LEFT = 8,
} qui_border;

typedef struct t_qui_widget qui_widget;

typedef struct t_qui_scroll_widget
{
	//s32 content_size;
} qui_scroll_widget;

typedef struct t_layout_widget
{
	s32 size;
	s32 fixed_size;
	s32 size_left_for_flex;
} layout_widget;

typedef enum t_qui_widget_state 
{
	IDLE,
	DOWN,
	HOVERED,
	OPEN,
} qui_widget_state;

typedef struct t_qui_scroll_button_widget
{
	bool is_up;
	qui_widget_state state;
} qui_scroll_button_widget;

typedef struct t_tabcontrol_panel 
{
	qui_widget_state state;
	char* text;
	qui_widget* container;
} tabcontrol_panel;

typedef struct t_qui_size_container
{
	color color_background;
	enum {
		DIRECTION_TOP,
	} direction;
	s32 min;
	s32 max;
	s32 drag_start_size;
	s32 mouse_drag_start_pos;
	qui_border border;
	u8 border_size;
} qui_size_container;

typedef struct t_qui_fixed_container 
{
	color color_background;
	qui_border border;
	u8 border_size;
} qui_fixed_container;

typedef struct t_qui_flex_container
{
	color color_background;
	qui_border border;
	u8 border_size;
	s32 min_size_px;
	u8 flex;
} qui_flex_container;

typedef struct t_qui_label
{
	char* text;
	bool allow_wrap;
	bool is_horizontal;
} qui_label;

typedef struct t_qui_dropdown
{
	qui_widget_state state;
	bool released;
	qui_widget* selected_child;
} qui_dropdown;

typedef struct t_qui_button 
{
	char* text;
	image* icon;
	qui_widget_state state;
	bool is_toggle;
	bool released;
} qui_button;

typedef struct t_qui_table_row
{
	array content;
	bool is_header;
} qui_table_row;

typedef struct t_qui_toolbar_item 
{
	char* text;
	image* icon;
	qui_widget_state state;
} qui_toolbar_item;

typedef struct t_qui_state
{
	qui_widget* dragging_widget;
	platform_window* window;
	s32 scissor_index;
	vec4 scissor_stack[100];
} qui_state;

typedef struct t_qui_widget 
{
	array children;
	array special_children; // Only used by master widget.
	struct t_qui_widget* parent;
	s32 width;
	s32 height;
	s16 x;
	s16 y;
	u8 margin_x;
	u8 margin_y;
	qui_widget_type type;
	u8* data; // Widget specific data.
	bool visible; // Currently only has effect on flex container.
	s16 scroll_y;
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
qui_widget* qui_create_flex_container_s(qui_widget* qui, u8 flex, s32 min_size_px);
qui_widget* qui_create_horizontal_layout(qui_widget* qui);
qui_widget* qui_create_dropdown(qui_widget* qui);
qui_widget* qui_create_itembar_with_border(qui_widget* qui, qui_border border);
qui_widget* qui_create_itembar(qui_widget* qui);
qui_widget* qui_create_label(qui_widget* qui, char* text, bool allow_wrap);

void qui_set_size_container_bounds(qui_widget* el, u32 min, u32 max);
void qui_flex_container_set_border(qui_widget* el, qui_border border, u8 border_size);
void qui_fixed_container_set_border(qui_widget* el, qui_border border, u8 border_size);

void qui_render(platform_window* window, qui_widget* qui);
void qui_update(platform_window* window, qui_widget* qui);

#endif