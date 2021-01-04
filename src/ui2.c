/* 
*  BSD 2-Clause “Simplified” License
*  Copyright (c) 2019, Aldrik Ramaekers, aldrik.ramaekers@protonmail.com
*  All rights reserved.
*/

#define PRE_DRAW(__el) if (!__el->redraw) goto done; else printf("DRAWING! %s\n", __func__);
#define POST_DRAW(__el) __el->redraw = false;\
    done:\
    render_children(__el);

#define update_children(p, _window)\
   for (s32 i = 0; i < p->children.length; i++)\
    {\
        ui_element* ___el = *(ui_element**)array_at(&p->children, i);\
        ___el->update(___el, _window);\
    }

#define render_children(p)\
   for (s32 i = 0; i < p->children.length; i++)\
    {\
        ui_element* ___el = *(ui_element**)array_at(&p->children, i);\
        ___el->render(___el);\
    }

void ui_element_add_child(ui_element* parent, ui_element* el)
{
    array_push(&parent->children, &el);
}

static void ui_set_element_size(ui_element* new_element, platform_window* window, u32 w, u32 h) {
    vec2 size = platform_get_window_size(window);
    if (w) {
        new_element->width = w;
    }
    else if (new_element->parent) {
        new_element->width = new_element->parent->width;
    }
    else {
        new_element->width = size.x;
    }

    if (h) {
        new_element->height = h;
    }
    else if (new_element->parent) {
        new_element->height = new_element->parent->height;
    }
    else {
        new_element->height = size.y;
    }
}

static s32 ui_get_sibling_total_x(ui_element* el)
{
    s32 x = 0;
    if (el->parent != 0)
    {
        for (s32 i = 0; i < el->children.length; i++)
        {
            ui_element* _el = *(ui_element**)array_at(&el->children, i);
            if (el == _el) break;
            x += _el->width;
        }
    }

    return x;
}

static s32 ui_get_parent_count(ui_element* el)
{
    s32 x = 0;
    while (el->parent) {
        x++;
        el = el->parent;
    }

    return x;
}

static vec2 ui_get_absolute_position(ui_element* el)
{
    vec2 result;
    u32 total_x = el->x;
    u32 total_y = el->y;

    while (el->parent) {
        total_x += el->parent->x;
        total_y += el->parent->y;
        el = el->parent;
    }

    result.x = total_x;
    result.y = total_y;

    return result;
}

////////////////////////////
////////////////////////////
////////////////////////////
ui* ui_create(platform_window* window)
{
    ui* new_element = mem_alloc(sizeof(ui));
    new_element->parent = 0;
    new_element->update = 0;
    new_element->render = 0;
    new_element->x = 0;
    new_element->y = 0;
    new_element->children = array_create(sizeof(ui_element*));
    new_element->window = window;
    new_element->redraw = true;
    return new_element;
}

void ui_update_render(ui* ui)
{
    platform_window_make_current(ui->window);
	platform_set_cursor(ui->window, CURSOR_DEFAULT);
	camera_apply_transformations(ui->window, &_global_camera);
    ui_set_element_size((ui_element*)ui, ui->window, 0, 0);
    update_children(ui, ui->window);
    
    // TODO: create double buffer because we cant edit buffer after platform_swap_buffers() has been called.
    PRE_DRAW(ui);
    {
        render_clear(ui->window);
    }
    POST_DRAW(ui);
}

////////////////////////////
////////////////////////////
////////////////////////////
static void ui_menu_update(ui_element* el, platform_window* window)
{
    ui_set_element_size(el, window, 0, 0);
    el->height = MENU_HEIGHT;

    update_children(el, window);
}

static void ui_menu_render(ui_element* el)
{
    PRE_DRAW(el);
    {
        vec2 pos = ui_get_absolute_position(el);

        render_rectangle(pos.x, pos.y, el->width, el->height, COLOR_BG);
        render_rectangle(pos.x, pos.y + el->height - 1, el->width, 1, COLOR_BORDER);
        render_rectangle(pos.x, 0, el->width, 1, COLOR_BORDER);
    }
    POST_DRAW(el);
}

ui_element* ui_menu_create(ui_element* parent)
{
    ui_element* new_element = mem_alloc(sizeof(ui_element));
    new_element->parent = parent;
    new_element->update = ui_menu_update;
    new_element->render = ui_menu_render;
    new_element->x = 0;
    new_element->y = 0;
    new_element->redraw = true;
    new_element->children = array_create(sizeof(ui_element*));

    if (parent) ui_element_add_child(parent, (ui_element*)new_element);

    return (ui_element*)new_element;
}

////////////////////////////
////////////////////////////
////////////////////////////
static void ui_menu_tab_update(ui_element* el, platform_window* window)
{
    if (ui_get_parent_count(el) == 1) {
        el->x = ui_get_sibling_total_x(el);
        el->y = 0;
    }

    vec2 pos = ui_get_absolute_position(el);

    if (_global_mouse.x >= pos.x && _global_mouse.x <= pos.x + el->width && _global_mouse.y >= pos.y && _global_mouse.y <= pos.y + el->height)
    {
        if (el->state != UI_STATE_HOVERED) {
            el->state = UI_STATE_HOVERED;
            el->redraw = true;
        }
    }
    else
    {
        if (el->state != UI_STATE_IDLE) {
            el->state = UI_STATE_IDLE;
            el->redraw = true;
        }
    }

    update_children(el, window);
}

static void ui_menu_tab_render(ui_element* el)
{
    PRE_DRAW(el);
    {
        ui_menu_item* menu_item = (ui_menu_item*)el;

        vec2 pos = ui_get_absolute_position(el);
        s32 text_y = (el->height / 2) - (global_ui_context.font_small->px_h / 2);

        color bg_color = COLOR_BG;
        color fg_color = COLOR_FG;
        if (menu_item->state == UI_STATE_HOVERED) {
            bg_color = COLOR_HOVER;
            fg_color = COLOR_FG_HOVER;
        }

        render_rectangle(pos.x, pos.y+1, el->width, el->height-2, bg_color);
        render_text(global_ui_context.font_small, pos.x + 10, pos.y + text_y, menu_item->text, fg_color);
    }
    POST_DRAW(el);
}

ui_element* ui_menu_add_tab(ui_element* parent, char* text)
{
    log_assert(parent, "Menu tab needs to have a parent element");

    ui_menu_item* new_element = mem_alloc(sizeof(ui_menu_item));
    new_element->parent = parent;
    new_element->update = ui_menu_tab_update;
    new_element->render = ui_menu_tab_render;
    new_element->x = 0;
    new_element->y = 0;
    new_element->height = MENU_HEIGHT;
    new_element->redraw = true;
    new_element->state = UI_STATE_IDLE;
    new_element->text = mem_alloc(strlen(text) + 1);
    string_copyn(new_element->text, text, strlen(text) + 1);
    new_element->width = calculate_text_width(global_ui_context.font_small, new_element->text) + 20;
    new_element->children = array_create(sizeof(ui_element*));

    if (parent) ui_element_add_child(parent, (ui_element*)new_element);

    return (ui_element*)new_element;
}