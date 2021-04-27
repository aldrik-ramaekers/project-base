/* 
*  BSD 2-Clause “Simplified” License
*  Copyright (c) 2019, Aldrik Ramaekers, aldrik.ramaekers@protonmail.com
*  All rights reserved.
*/

u32 _hash_type_name(char *str)
{
    u32 hash = 5381;
    int c;

    while ((c = *str++))
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash;
}

static ui_el_base _ui_create_base()
{
	ui_el_base base;
	base.type = 0;
	base.parent = 0;
	base.dock = 0;
	base.size = 0;
	base.dir = DIRECTION_VERTICAL;
	base.size_ref = el_size(1);
	base.children = array_create(sizeof(ui_el_base));
	array_reserve(&base.children, 10);
	base.area = (vec4){0,0,0,0};

	return base;
}

#define get_type(_type) #_type

#define create_base_for(_cont, _type)\
	ui_el_base base = _ui_create_base();\
	_type *container_ptr = (_type*)(&base);\
	container_ptr->type = #_type;\
	container_ptr->size = sizeof(_type);\
	_cont = *container_ptr;

ui_el_container* ui_create(platform_window* window)
{
	ui_el_container* container = (ui_el_container*)mem_alloc(sizeof(ui_el_base));
	ui_el_base base = _ui_create_base();
	memcpy(container, &base, sizeof(ui_el_base));
	container->type = get_type(ui_el_container);
	container->size = sizeof(ui_el_container);

	container->dock = DOCK_ALL;
	container->area = camera_get_target_rectangle(window, &_global_camera);

	return container;
}

ui_el_layout* ui_create_layout(el_size size_ref, ui_el_base* parent, ui_direction dir)
{
	ui_el_layout container;
	create_base_for(container, ui_el_layout);
	container.size_ref = size_ref;
	container.dir = dir;
	container.parent = parent;
	int index = array_push_size(&parent->children, &container, container.size);
	return (ui_el_layout*)array_at(&parent->children, index);
}

ui_el_scrollable* ui_create_scrollable(el_size size_ref, ui_el_base* parent)
{
	ui_el_scrollable container;
	create_base_for(container, ui_el_scrollable);
	container.size_ref = size_ref;
	container.dir = DIRECTION_VERTICAL;
	container.parent = parent;
	int index = array_push_size(&parent->children, &container, container.size);
	return (ui_el_scrollable*)array_at(&parent->children, index);
}

ui_el_container* ui_create_container(el_size size_ref, ui_el_base* parent)
{
	ui_el_container container;
	create_base_for(container, ui_el_container);
	container.size_ref = size_ref;
	container.dir = DIRECTION_VERTICAL;
	container.parent = parent;
	int index = array_push_size(&parent->children, &container, container.size);
	return (ui_el_container*)array_at(&parent->children, index);
}

static bool _is_element_size_independent(ui_el_base* el)
{
	return  el->size_ref.min_w || el->size_ref.min_h ||  el->size_ref.max_w ||  el->size_ref.max_h;
}

static s32 _calculate_position_for_el(ui_el_base* el, int current_offset)
{
	if (el->parent->dir == DIRECTION_HORIZONTAL) {
		el->area.x = el->parent->area.x + current_offset;
		el->area.y = el->parent->area.y;
		current_offset += el->area.w;
	}
	else {
		el->area.y = el->parent->area.y + current_offset;
		el->area.x = el->parent->area.x;
		current_offset += el->area.h;
	}

	return current_offset;
}

static void _calculate_size_for_dependent_el(ui_el_base* el, float flex_1)
{
	if (el->parent->dir == DIRECTION_HORIZONTAL) {
		float new_s = el->size_ref.flex * flex_1;
		el->area.h = el->parent->area.h;
		el->area.w  = round(new_s);
	}
	else {
		float new_s = el->size_ref.flex * flex_1;
		el->area.w = el->parent->area.w;
		el->area.h  = round(new_s);
	}
}

static void _calculate_size_for_independent_el(ui_el_base* el, float flex_1)
{
	if (el->parent->dir == DIRECTION_HORIZONTAL) {
		float new_s = el->size_ref.flex * flex_1;
		el->area.h = el->parent->area.h;
		el->area.w  = round(new_s);
	}
	else {
		float new_s = el->size_ref.flex * flex_1;
		el->area.w = el->parent->area.w;
		el->area.h  = round(new_s);
	}

	if (el->area.w > el->size_ref.max_w && el->size_ref.max_w) el->area.w = el->size_ref.max_w;
	if (el->area.h > el->size_ref.max_h && el->size_ref.max_h) el->area.h = el->size_ref.max_h;
	if (el->area.w < el->size_ref.min_w && el->size_ref.min_w) el->area.w = el->size_ref.min_w;
	if (el->area.h < el->size_ref.min_h && el->size_ref.min_h) el->area.h = el->size_ref.min_h;
}

static void _handle_resizing(ui_el_base* el)
{
	float total_flex = 0;
	float flex_1 = 0.0f;
	int area_size = (el->dir == DIRECTION_HORIZONTAL) ? el->area.w : el->area.h;

	for (s32 i = 0; i < el->children.length; i++)
	{
		ui_el_base* child = array_at(&el->children, i);
		total_flex += child->size_ref.flex;
	}

	flex_1 = area_size / total_flex;
	// 1. calculate independent children
	for (s32 i = 0; i < el->children.length; i++)
	{
		ui_el_base* child = array_at(&el->children, i);
		if (_is_element_size_independent(child)) {
			_calculate_size_for_independent_el(child, flex_1);

			// Remove area that has been calculated from available area.
			total_flex -= child->size_ref.flex;
			if (el->dir == DIRECTION_HORIZONTAL) {
				area_size -= child->area.w;
			}
			else {
				area_size -= child->area.h;
			}
		}
	}

	flex_1 = area_size / total_flex;
	// 2. calculate independent children
	for (s32 i = 0; i < el->children.length; i++)
	{
		ui_el_base* child = array_at(&el->children, i);
		if (!_is_element_size_independent(child))
			_calculate_size_for_dependent_el(child, flex_1);
	}

	// 3. calculate position
	s32 current_offset = 0;
	for (s32 i = 0; i < el->children.length; i++)
	{
		ui_el_base* child = array_at(&el->children, i);
		current_offset = _calculate_position_for_el(child, current_offset);
	}
}

static void _ui_el_resize_sub(ui_el_base* el, u32 x, u32 y, u32 w, u32 h)
{
	if (el->parent == 0) {
		if (w > 0) {
			el->area.w += w;
		}
		if (h > 0) {
			el->area.h += h;
		}
	}
	
	_handle_resizing(el);

	for (s32 i = 0; i < el->children.length; i++)
	{
		ui_el_base* child = array_at(&el->children, i);
		_ui_el_resize_sub(child, 0,0,0,0);
	}
}

void ui_el_resize(ui_el_base* el, u32 change_x, u32 change_y)
{
	_ui_el_resize_sub(el, 0, 0, change_x, change_y);	
}

static vec4 _get_bounds_for_el(ui_el_base* el)
{
	vec4 vec = el->area;

	ui_el_base* p = el->parent;
	while(p)
	{
		if (vec.x < p->area.x) vec.x = p->area.x;
		if (vec.y < p->area.y) vec.y = p->area.y;
		if (vec.x+vec.w > p->area.x+p->area.w) vec.w = (vec.x+vec.w) - (vec.x + vec.w - p->area.x + p->area.w);
		if (vec.y+vec.h > p->area.y+p->area.h) vec.h = (vec.y+vec.h) - (vec.y + vec.h - p->area.y + p->area.h);
		p = p->parent;
	}
	return vec;
}

void ui_el_render(ui_el_base* el, platform_window* window)
{
	color c = rgb(255, 255, 255);
	if ((void*)el->type == (void*)get_type(ui_el_container)) c = rgb(200,0,0);
	if ((void*)el->type == (void*)get_type(ui_el_layout)) c = rgb(0,200,0);
	if ((void*)el->type == (void*)get_type(ui_el_scrollable)) c = rgb(0,0,200);

	vec4 v = _get_bounds_for_el(el);
	renderer->render_set_scissor(window, v.x,v.y,v.w,v.h);
	renderer->render_rectangle_outline(el->area.x, el->area.y, el->area.w, el->area.h, 1, c);
	renderer->render_reset_scissor();

#if 1
	char size[20];
	sprintf(size, "x: %d, y: %d", el->area.w, el->area.h);
	renderer->render_text(global_ui_context.font_small, el->area.x, el->area.y, size, rgb(0,0,0));
#endif

	for (s32 i = 0; i < el->children.length; i++)
	{
		ui_el_base* child = array_at(&el->children, i);
		ui_el_render(child, window);
	}
}

void ui_print_tree(ui_el_base* el, s32 indent)
{
	for (s32 i = 0; i < indent; i++) {
		printf("  ");
	}
	printf("%s {x: %d, y: %d, w: %d, h: %d}\n", el->type, el->area.x, el->area.y, el->area.w, el->area.h);
	for (s32 i = 0; i < el->children.length; i++)
	{
		ui_el_base* child = array_at(&el->children, i);
		ui_print_tree(child, indent+1);
	}
}