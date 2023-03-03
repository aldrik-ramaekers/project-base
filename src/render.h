/* 
*  BSD 2-Clause “Simplified” License
*  Copyright (c) 2019, Aldrik Ramaekers, aldrik.ramaekers@protonmail.com
*  All rights reserved.
*/

#ifndef INCLUDE_RENDER
#define INCLUDE_RENDER

typedef struct t_color {
	u8 r;
	u8 g;
	u8 b;
	u8 a;
} color;

typedef struct t_render_target
{
	s32 x;
	s32 y;
	s32 w;
	s32 h;
	
	s32 offset_x;
	s32 offset_y;
} render_target;

typedef enum t_triangle_direction
{
	TRIANGLE_DOWN,
	TRIANGLE_UP,
	TRIANGLE_LEFT,
	TRIANGLE_RIGHT,
} triangle_direction;

typedef enum t_render_driver_id
{
	DRIVER_NONE,
	DRIVER_GL,
	DRIVER_CPU,
} render_driver_id;

typedef struct t_render_driver
{
	char*	name;
	void 	(*set_render_depth)(s32 depth);
	void 	(*render_clear)(platform_window *window, color tint);
	void 	(*render_image)(image *image, s32 x, s32 y, s32 width, s32 height);
	void 	(*render_image_tint)(image *image, s32 x, s32 y, s32 width, s32 height, color tint);
	void	(*render_image_quad)(image *image, s32 x1, s32 y1, s32 x2, s32 y2, s32 x3, s32 y3, s32 x4, s32 y4);
	void	(*render_image_quad_tint)(image *image, s32 x1, s32 y1, s32 x2, s32 y2, s32 x3, s32 y3, s32 x4, s32 y4, color tint);
	void 	(*render_image_quad_partial)(image *image, s32 x1, s32 y1, s32 x2, s32 y2, s32 x3, s32 y3, s32 x4, s32 y4, vec2f tl, vec2f tr, vec2f bl, vec2f br);
	void 	(*render_glyph)(font* font, s32 px, s32 py, glyph g, color tint);
	s32 	(*render_text)(font *font, s32 x, s32 y, char *text, color tint);
	s32 	(*render_text_ellipsed)(font *font, s32 x, s32 y, s32 maxw, char *text, color tint);
	s32 	(*render_text_cutoff)(font *font, s32 x, s32 y, char *text, color tint, u16 cutoff_width);
	s32 	(*render_text_with_cursor)(font *font, s32 x, s32 y, char *text, color tint, s32 cursor_pos);
	s32 	(*render_text_with_selection)(font *font, s32 x, s32 y, char *text, color tint, s32 selection_start, s32 selection_length);
	s32 	(*calculate_cursor_position)(font *font, char *text, s32 click_x);
	s32 	(*calculate_text_width)(font *font, char *text);
	s32 	(*calculate_text_width_upto)(font *font, char *text, s32 index);
	s32 	(*calculate_text_width_from_upto)(font *font, char *text, s32 from, s32 index);
 	void 	(*render_tri)(s32 x1, s32 y1, s32 x2, s32 y2, s32 x3, s32 y3, color tint);
	void 	(*render_quad)(s32 x1, s32 y1, s32 x2, s32 y2, s32 x3, s32 y3, s32 x4, s32 y4, color tint);
	void 	(*render_rectangle)(s32 x, s32 y, s32 width, s32 height, color tint);
	void 	(*render_line)(s32 x1, s32 y1, s32 x2, s32 y2, float thickness, color tint);
	void 	(*render_rectangle_outline)(s32 x, s32 y, s32 width, s32 height, u16 outline_w, color tint);
	void 	(*render_triangle)(s32 x, s32 y, s32 w, s32 h, color tint, triangle_direction dir);
	void 	(*render_set_scissor)(platform_window *window, s32 x, s32 y, s32 w, s32 h);
	vec4 	(*render_get_scissor)();
	void 	(*render_reset_scissor)();
	void 	(*render_set_rotation)(float32 rotation);
	void	(*render_reset_rotation)(float32 rotation, float32 x, float32 y);
	void 	(*render_arc)(float x1, float y1, float x2, float y2, float radius, bool arcDirection, bool useBiggerArc, color c, float thickness);
	s32 	(*render_text_rd)(font *font, s32 x, s32 y, char *text, color tint, u16 target_h);
	void	(*render_rounded_rectangle)(float x, float y, float width, float height, color color, float radius, int innerPad);
} render_driver;

// Current driver
render_driver* renderer = 0;
platform_window* drawing_window = 0;

render_driver_id current_render_driver();
void set_render_driver(render_driver_id driver);

#define rgb(r_,g_,b_) (color){ r_, g_, b_, 255 }
#define rgba(r_,g_,b_,a_) (color){r_,g_,b_,a_}

#endif