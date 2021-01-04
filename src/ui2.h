/* 
*  BSD 2-Clause “Simplified” License
*  Copyright (c) 2019, Aldrik Ramaekers, aldrik.ramaekers@protonmail.com
*  All rights reserved.
*/

#ifndef INCLUDE_UI2
#define INCLUDE_UI2

#define MENU_HEIGHT 20

#define COLOR_BG rgb(221, 221, 226)
#define COLOR_BORDER rgb(165, 165, 178)
#define COLOR_HOVER rgb(90, 97, 111)

#define COLOR_FG rgb(24, 18, 18)
#define COLOR_FG_HOVER rgb(255, 255, 255)

typedef enum t_ui_element_state
{
    UI_STATE_IDLE,
    UI_STATE_HOVERED,
    UI_STATE_OPEN,
    UI_STATE_DISABLED,
} ui_element_state;

typedef struct t_ui_element ui_element;

#define UI_ELEMENT_BASE\
    ui_element* parent;\
    array children;\
    u32 x;\
    u32 y;\
    u32 width;\
    u32 height;\
    ui_element_state state;\
    bool redraw;\
    void (*render)(ui_element*);\
    void (*update)(ui_element*,platform_window*);

typedef struct t_ui_element
{
    UI_ELEMENT_BASE;
} ui_element;

typedef struct t_ui
{
    UI_ELEMENT_BASE;
    platform_window *window;
} ui;

typedef struct t_ui_horizontal_splitter
{
    UI_ELEMENT_BASE;
    u32 min_x;
    u32 max_x;
} ui_horizontal_splitter;

typedef struct t_ui_menu_item
{
    UI_ELEMENT_BASE;
    char* text;
} ui_menu_item;


ui* ui_create(platform_window* window);
void ui_update_render(ui* ui);

void ui_element_add_child(ui_element* parent, ui_element* el);

ui_element* ui_menu_create(ui_element* parent);
ui_element* ui_menu_add_tab(ui_element* parent, char* text);

ui_element* ui_horizontal_splitter_create(ui_element* parent);
// ui_element* ui_checkbox_create(ui_element* parent, platform_window* window, u32 x, u32 y);

#endif