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
	base.dir = DIRECTION_HORIZONTAL;
	base.flex = FLEX_GROW;
	base.children = array_create(sizeof(ui_el_base));
	array_reserve(&base.children, 10);
	base.area = (vec4){0,0,0,0};

	return base;
}

#define get_type(_type) _hash_type_name(#_type)

#define create_base_for(_cont, _type)\
	ui_el_base base = _ui_create_base();\
	_type *container_ptr = (_type*)(&base);\
	container_ptr->type = _hash_type_name(#_type);\
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

ui_el_layout* ui_create_layout(u8 flex, ui_el_base* parent, ui_direction dir)
{
	ui_el_layout container;
	create_base_for(container, ui_el_layout);
	container.flex = flex;
	container.dir = dir;
	container.parent = parent;
	int index = array_push_size(&parent->children, &container, container.size);
	return (ui_el_layout*)array_at(&parent->children, index);
}

ui_el_container* ui_create_container(u8 flex, ui_el_base* parent)
{
	ui_el_container container;
	create_base_for(container, ui_el_container);
	container.flex = flex;
	container.dir = parent->dir;
	container.parent = parent;
	int index = array_push_size(&parent->children, &container, container.size);
	return (ui_el_container*)array_at(&parent->children, index);
}

static void _ui_el_resize_sub(ui_el_base* el, u32 x, u32 y, u32 w, u32 h)
{
	u32 change_x = 0;
	u32 change_y = 0;
	u32 change_w = 0;
	u32 change_h = 0;

	if (w > 0) {
		if (el->dock & DOCK_RIGHT && el->dock & DOCK_LEFT)
		{
			change_w = w;
			el->area.w += w;
		}
		else if (el->dock & DOCK_RIGHT)
		{
			change_x = w;
			el->area.x += w;
		}
	}
	if (h > 0) {
		if (el->dock & DOCK_TOP && el->dock & DOCK_BOTTOM)
		{
			change_h = h;
			el->area.h += h;
		}
		else if (el->dock & DOCK_RIGHT)
		{
			change_y = h;
			el->area.y += h;
		}
	}
	
	if (el->parent)
	{
		s32 total_flex = 0;
		int my_index = 0;

		for (s32 i = 0; i < el->parent->children.length; i++)
		{
			ui_el_base* child = array_at(&el->parent->children, i);
			total_flex += child->flex;
			if (child == el) my_index = i;
		}

		if (el->parent->dir == DIRECTION_HORIZONTAL) {
			s32 width_sep = el->parent->area.w / total_flex;
			s32 new_width = width_sep * el->flex;
			el->area.w = new_width;
			el->area.h = el->parent->area.h;

			el->area.x = el->parent->area.x;
			if (my_index > 0) {
				ui_el_base* prev_sib = (ui_el_base*)array_at(&el->parent->children, my_index-1);
				el->area.x = prev_sib->area.x + prev_sib->area.w;
			}
			el->area.y = el->parent->area.y;
		}
		else {
			s32 height_sep = el->parent->area.h / total_flex;
			s32 new_height = height_sep * el->flex;
			el->area.h = new_height;
			el->area.w = el->parent->area.w;

			el->area.y = el->parent->area.y;
			if (my_index > 0) {
				ui_el_base* prev_sib = (ui_el_base*)array_at(&el->parent->children, my_index-1);
				el->area.y = prev_sib->area.y + prev_sib->area.h;
			}
			el->area.x = el->parent->area.x;
		}
	}

	for (s32 i = 0; i < el->children.length; i++)
	{
		ui_el_base* child = array_at(&el->children, i);
		_ui_el_resize_sub(child, change_x, change_y, change_w, change_h);
	}
}

void ui_el_resize(ui_el_base* el, u32 change_x, u32 change_y)
{
	_ui_el_resize_sub(el, 0, 0, change_x, change_y);	
}

void ui_el_render(ui_el_base* el)
{
	color c = rgb(255, 255, 255);
	if (el->type == get_type(ui_el_container)) c = rgb(200,0,0);
	if (el->type == get_type(ui_el_layout)) c = rgb(0,200,0);

	renderer->render_rectangle(el->area.x, el->area.y, el->area.w, el->area.h, c);

#if 1
	char size[20];
	sprintf(size, "x: %d, y: %d", el->area.w, el->area.h);
	renderer->render_text(global_ui_context.font_small, el->area.x, el->area.y, size, rgb(0,0,0));
#endif

	for (s32 i = 0; i < el->children.length; i++)
	{
		ui_el_base* child = array_at(&el->children, i);
		ui_el_render(child);
	}
}

void ui_print_tree(ui_el_base* el, s32 indent)
{
	for (s32 i = 0; i < indent; i++) {
		printf(" ");
	}
	printf("%d\n", el->type);
	for (s32 i = 0; i < el->children.length; i++)
	{
		indent++;
		ui_el_base* child = array_at(&el->children, i);
		ui_print_tree(child, indent+1);
	}
}