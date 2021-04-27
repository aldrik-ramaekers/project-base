/* 
*  BSD 2-Clause “Simplified” License
*  Copyright (c) 2019, Aldrik Ramaekers, aldrik.ramaekers@protonmail.com
*  All rights reserved.
*/

#ifndef INCLUDE_INTERFACE
#define INCLUDE_INTERFACE

#define MAX_CHILD_ELEMENTS 10
#define FLEX_GROW 0

typedef enum t_ui_dock
{
	DOCK_LEFT = 1,
	DOCK_TOP = 2,
	DOCK_RIGHT = 4,
	DOCK_BOTTOM = 8,

	DOCK_ALL = 15,
} ui_dock;

typedef struct t_el_size
{
	float flex;
	int max_w;
	int min_w;
	int max_h;
	int min_h;
} el_size;

#define el_size(_num) (el_size){_num,0,0,0,0}
#define el_size_w(_num,_miw,_maw) (el_size){_num,_maw,_miw,0,0}
#define el_size_h(_num,_mih,_mah) (el_size){_num,0,0,_mah,_mih}

typedef enum t_ui_direction
{
	DIRECTION_HORIZONTAL,
	DIRECTION_VERTICAL,
} ui_direction;

typedef struct t_ui_el_base ui_el_base;

#define UI_BASE\
	char* type;\
	u16 size;\
	ui_el_base* parent;\
	array children;\
	ui_dock dock;\
	vec4 area;\
	el_size size_ref;\
	ui_direction dir;

typedef struct t_ui_el_base
{
	UI_BASE;
	s32 filler[50];
} ui_el_base;

typedef struct t_ui_el_layout
{
	UI_BASE;
} ui_el_layout;

typedef struct t_ui_el_scrollable
{
	UI_BASE;
	u32 scrollable_area_size;
} ui_el_scrollable;

typedef struct t_ui_el_container
{
	UI_BASE;
} ui_el_container;

ui_el_container* ui_create();
ui_el_layout* ui_create_layout(el_size size_ref, ui_el_base* parent, ui_direction dir);
ui_el_container* ui_create_container(el_size size_ref, ui_el_base* parent);
ui_el_scrollable* ui_create_scrollable(el_size size_ref, ui_el_base* parent);

void ui_el_render(ui_el_base* el);
void ui_el_resize(ui_el_base* el, u32 change_x, u32 change_y);
void ui_print_tree(ui_el_base* el, s32 indent);

#endif
