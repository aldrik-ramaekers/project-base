/* 
*  BSD 2-Clause “Simplified” License
*  Copyright (c) 2019, Aldrik Ramaekers, aldrik.ramaekers@protonmail.com
*  All rights reserved.
*/

static u8 cpu_render_depth = 1;
static vec4 current_scissor;

static platform_window *drawing_window = 0;

static void _copy_image_pixel_with_tint(s32 x, s32 y, image *image, render_target rec, color tint)
{
    s32 offset = (y * (drawing_window->backbuffer.width) * 5) + x * 5;
    u8 *buffer_entry = drawing_window->backbuffer.buffer + offset;

    if (buffer_entry[4] > cpu_render_depth)
        return;
    buffer_entry[4] = cpu_render_depth;

    s32 _x = x - rec.x + rec.offset_x;
    s32 _y = y - rec.y + rec.offset_y;
    s32 image_offset = (_y * (image->width) * 4) + _x * 4;
    u8 *color = image->data + image_offset;

    float32 alpha = color[3] / 255.0f;
    float32 oneminusalpha = 1 - alpha;

    u8 b = ((tint.b * alpha) + (oneminusalpha * buffer_entry[0]));
    u8 g = ((tint.g * alpha) + (oneminusalpha * buffer_entry[1]));
    u8 r = ((tint.r * alpha) + (oneminusalpha * buffer_entry[2]));
    u8 a = color[3];

    s32 c = (a << 24) | (r << 16) | (g << 8) | (b << 0);

    memcpy(buffer_entry, &c, 4);
}

static void _copy_image_pixel(s32 x, s32 y, image *image, render_target rec)
{
    s32 offset = (y * (drawing_window->backbuffer.width) * 5) + x * 5;
    u8 *buffer_entry = drawing_window->backbuffer.buffer + offset;

    if (buffer_entry[4] > cpu_render_depth)
        return;
    buffer_entry[4] = cpu_render_depth;

    s32 _x = x - rec.x + rec.offset_x;
    s32 _y = y - rec.y + rec.offset_y;
    s32 image_offset = (_y * (image->width) * 4) + _x * 4;
    u8 *color = image->data + image_offset;

    float32 alpha = color[3] / 255.0f;
    float32 oneminusalpha = 1 - alpha;

    u8 b = ((color[0] * alpha) + (oneminusalpha * buffer_entry[0]));
    u8 g = ((color[1] * alpha) + (oneminusalpha * buffer_entry[1]));
    u8 r = ((color[2] * alpha) + (oneminusalpha * buffer_entry[2]));
    u8 a = color[3];

    s32 c = (a << 24) | (r << 16) | (g << 8) | (b << 0);

    memcpy(buffer_entry, &c, 4);
}

static void _copy_glyph_pixel(s32 x, s32 y, glyph *gl, render_target rec, color tint)
{
    s32 offset = (y * (drawing_window->backbuffer.width) * 5) + x * 5;
    u8 *buffer_entry = drawing_window->backbuffer.buffer + offset;

    if (buffer_entry[4] > cpu_render_depth)
        return;
    buffer_entry[4] = cpu_render_depth;

    s32 _x = x - rec.x + rec.offset_x;
    s32 _y = y - rec.y + rec.offset_y;
    s32 image_offset = (_y * gl->width) + _x;
    u8 *color = gl->bitmap + image_offset;

    float32 alpha = color[0] / 255.0f;
    float32 oneminusalpha = 1 - alpha;

    u8 r = ((tint.r * alpha) + (oneminusalpha * buffer_entry[2]));
    u8 g = ((tint.g * alpha) + (oneminusalpha * buffer_entry[1]));
    u8 b = ((tint.b * alpha) + (oneminusalpha * buffer_entry[0]));
    u8 a = color[0];

    s32 c = (a << 24) | (r << 16) | (g << 8) | (b << 0);

    memcpy(buffer_entry, &c, 4);
}

static void _set_pixel(s32 x, s32 y, color tint)
{
    s32 offset = (y * (drawing_window->backbuffer.width) * 5) + x * 5;
    u8 *buffer_entry = drawing_window->backbuffer.buffer + offset;

    if (buffer_entry[4] > cpu_render_depth)
        return;
    buffer_entry[4] = cpu_render_depth;

    float32 alpha = tint.a / 255.0f;
    float32 oneminusalpha = 1 - alpha;

    u8 r = ((tint.r * alpha) + (oneminusalpha * buffer_entry[2]));
    u8 g = ((tint.g * alpha) + (oneminusalpha * buffer_entry[1]));
    u8 b = ((tint.b * alpha) + (oneminusalpha * buffer_entry[0]));
    u8 a = tint.a;

    s32 c = (a << 24) | (r << 16) | (g << 8) | (b << 0);

    memcpy(buffer_entry, &c, 4);
}

// returns topleft and bottomright corners. not width + height
static render_target _get_actual_rect(s32 x, s32 y, s32 width, s32 height)
{
    if (x < 0)
        x = 0;
    if (y < 0)
        y = 0;
    s32 start_x = x;
    s32 start_y = y;
    s32 end_x = start_x + width;
    s32 end_y = start_y + height;
    s32 offset_x = 0;
    s32 offset_y = 0;

    if (start_x < current_scissor.x)
    {
        offset_x = current_scissor.x - start_x;
        start_x = current_scissor.x;
    }
    if (start_y < current_scissor.y)
    {
        offset_y = current_scissor.y - start_y;
        start_y = current_scissor.y;
    }

    if (end_x > current_scissor.x + current_scissor.w)
        end_x = current_scissor.x + current_scissor.w;
    if (end_y > current_scissor.y + current_scissor.h)
        end_y = current_scissor.y + current_scissor.h;

    if (end_x >= drawing_window->backbuffer.width)
        end_x = drawing_window->backbuffer.width;
    if (end_y >= drawing_window->backbuffer.height)
        end_y = drawing_window->backbuffer.height;

    return (render_target){start_x, start_y, end_x, end_y, offset_x, offset_y};
}

static void cpu_render_reset_scissor()
{
    current_scissor = (vec4){0, 0, drawing_window->width + 1, drawing_window->height + 1};
}

static void cpu_render_rectangle(s32 x, s32 y, s32 width, s32 height, color tint)
{
    render_target rec = _get_actual_rect(x, y, width, height);

    for (s32 y = rec.y; y < rec.h; y++)
    {
        for (s32 x = rec.x; x < rec.w; x++)
        {
            _set_pixel(x, y, tint);
        }
    }
}

static void cpu_render_clear(platform_window *window, color tint)
{
    drawing_window = window;
    cpu_render_reset_scissor();

    cpu_render_depth = 1;

    u8 pixel[5] = {tint.r,tint.g,tint.b, 0xFF, 0x00};
    s32 pixel_count = window->backbuffer.width * window->backbuffer.height;
    for (s32 i = 0; i < pixel_count; i++)
        memcpy(window->backbuffer.buffer + (i * 5), pixel, 5);
}

static void cpu_render_set_rotation(float32 rotation)
{
    log_assert(0, "render_set_rotation not implemented on cpu");
}

static void cpu_render_reset_rotation(float32 rotation, float32 x, float32 y)
{
    log_assert(0, "render_reset_rotation not implemented on cpu");
}

static void cpu_set_cpu_render_depth(s32 depth)
{
    cpu_render_depth = depth;
}

static void cpu_render_image(image *image, s32 x, s32 y, s32 width, s32 height)
{
    log_assert(image, "Image cannot be null");

    if (image->loaded)
    {
        render_target rec = _get_actual_rect(x, y, image->width, image->height);

        for (s32 y = rec.y; y < rec.h; y++)
        {
            for (s32 x = rec.x; x < rec.w; x++)
            {
                _copy_image_pixel(x, y, image, rec);
            }
        }
    }
}

static void cpu_render_image_tint(image *image, s32 x, s32 y, s32 width, s32 height, color tint)
{
    log_assert(image, "Image cannot be null");

    if (image->loaded)
    {
        render_target rec = _get_actual_rect(x, y, image->width, image->height);

        for (s32 y = rec.y; y < rec.h; y++)
        {
            for (s32 x = rec.x; x < rec.w; x++)
            {
				_copy_image_pixel_with_tint(x, y, image, rec, tint);
            }
        }
    }
}

static s32 cpu_render_text_ellipsed(font *font, s32 x, s32 y, s32 maxw, char *text, color tint)
{
    if (!font->loaded)
        return 0;

    char *ellipse = "...";
    bool in_ellipse = false;

    s32 len = utf8len(text);

    s32 x_ = x;
    utf8_int32_t ch;
    s32 count = 0;
    while ((text = utf8codepoint(text, &ch)) && ch)
    {
        count++;
        if (ch == 9)
            ch = 32;
        utf8_int32_t ch_next;
        utf8codepoint(text, &ch_next);
        if (ch < TEXT_CHARSET_START || ch > TEXT_CHARSET_END)
        {
            ch = 0x3f;
        }
        if (ch == '\n')
            ch = 0xB6;

        glyph g = font->glyphs[ch];

        s32 y_ = y + font->px_h + g.yoff;
        s32 x_to_render = x_ + (g.lsb);

        render_target rec = _get_actual_rect(x_to_render, y_, g.width, g.height);

        for (s32 y = rec.y; y < rec.h; y++)
        {
            for (s32 x = rec.x; x < rec.w; x++)
            {
                _copy_glyph_pixel(x, y, &g, rec, tint);
            }
        }

        /* add kerning */
        //kern = stbtt_GetCodepointKernAdvance(&font->info, ch, ch_next);
        x_ += (g.advance);

        if (!in_ellipse && (x_ - x) > maxw - (font->glyphs['.'].width * 3) && count < len - 3)
        {
            in_ellipse = true;
            text = ellipse;
        }
    }

    return maxw;
}

static s32 cpu_render_text_with_selection(font *font, s32 x, s32 y, char *text, color tint, s32 selection_start, s32 selection_length)
{
    if (!font->loaded)
        return 0;

    s32 x_ = x;
    utf8_int32_t ch;
    s32 index = 0;
    s32 selection_start_x = x_;
    s32 selection_end_x = x_;
    while ((text = utf8codepoint(text, &ch)) && ch)
    {
        if (ch == 9)
            ch = 32;
        utf8_int32_t ch_next;
        utf8codepoint(text, &ch_next);
        if (ch < TEXT_CHARSET_START || ch > TEXT_CHARSET_END)
        {
            ch = 0x3f;
        }
        if (ch == '\n')
            ch = 0xB6;

        glyph g = font->glyphs[ch];

        s32 y_ = y + font->px_h + g.yoff;
        s32 x_to_render = x_ + (g.lsb);

        render_target rec = _get_actual_rect(x_to_render, y_, g.width, g.height);

        for (s32 y = rec.y; y < rec.h; y++)
        {
            for (s32 x = rec.x; x < rec.w; x++)
            {
                _copy_glyph_pixel(x, y, &g, rec, tint);
            }
        }

        /* add kerning */
        //kern = stbtt_GetCodepointKernAdvance(&font->info, ch, ch_next);
        x_ += (g.advance);

        index++;
        if (index == selection_start)
        {
            selection_start_x = x_;
        }
        if (index == selection_start + selection_length)
        {
            selection_end_x = x_;
        }
    }

	// TODO: give selection height as parameter
	(void)selection_start_x;
	(void)selection_end_x;
    //cpu_render_rectangle(selection_start_x, y - 3, selection_end_x - selection_start_x, TEXTBOX_HEIGHT - 10, rgba(66, 134, 244, 120));

    return x_ - x;
}

static s32 cpu_render_text_with_cursor(font *font, s32 x, s32 y, char *text, color tint, s32 cursor_pos)
{
    if (!font->loaded)
        return 0;

    float x_ = x;
    utf8_int32_t ch;
    s32 index = 0;
    s32 cursor_x = x_;
    while ((text = utf8codepoint(text, &ch)) && ch)
    {
        if (ch == 9)
            ch = 32;
        utf8_int32_t ch_next;
        utf8codepoint(text, &ch_next);
        if (ch < TEXT_CHARSET_START || ch > TEXT_CHARSET_END)
        {
            ch = 0x3f;
        }
        if (ch == '\n')
            ch = 0xB6;

        glyph g = font->glyphs[ch];

        s32 y_ = y + font->px_h + g.yoff;
        s32 x_to_render = x_ + (g.lsb);

        render_target rec = _get_actual_rect(x_to_render, y_, g.width, g.height);

        for (s32 y = rec.y; y < rec.h; y++)
        {
            for (s32 x = rec.x; x < rec.w; x++)
            {
                _copy_glyph_pixel(x, y, &g, rec, tint);
            }
        }

        /* add kerning */
        //kern = stbtt_GetCodepointKernAdvance(&font->info, ch, ch_next);
        x_ += (g.advance);
        index++;

        if (index == cursor_pos)
        {
            cursor_x = x_;
        }
    }

	// TODO: pass cursor color as parameter
	(void)cursor_x;
    //cpu_render_rectangle(cursor_x, y - 3, 2, TEXTBOX_HEIGHT - 10, global_ui_context.style.textbox_foreground);

    return x_ - x;
}

static s32 cpu_render_text(font *font, s32 x, s32 y, char *text, color tint)
{
    if (!font->loaded)
        return 0;

    s32 x_ = x;
    utf8_int32_t ch;
    while ((text = utf8codepoint(text, &ch)) && ch)
    {
        if (ch == 9)
            ch = 32;
        utf8_int32_t ch_next;
        utf8codepoint(text, &ch_next);
        if (ch < TEXT_CHARSET_START || ch > TEXT_CHARSET_END)
        {
            ch = 0x3f;
        }
        if (ch == 10)
            ch = 0xB6;
        if (ch == 13)
            continue;

        glyph g = font->glyphs[ch];

        s32 y_ = y + font->px_h + g.yoff;
        s32 x_to_render = x_ + (g.lsb);

        render_target rec = _get_actual_rect(x_to_render, y_, g.width, g.height);

        for (s32 y = rec.y; y < rec.h; y++)
        {
            for (s32 x = rec.x; x < rec.w; x++)
            {
                _copy_glyph_pixel(x, y, &g, rec, tint);
            }
        }

        /* add kerning */
        //kern = stbtt_GetCodepointKernAdvance(&font->info, ch, ch_next);
        x_ += (g.advance);
    }

    return x_ - x;
}

static s32 cpu_render_text_cutoff(font *font, s32 x, s32 y, char *text, color tint, u16 cutoff_width)
{
    if (!font->loaded)
        return 0;

    s32 x_ = x;
    s32 y_ = y;
    bool is_new_line = false;
    utf8_int32_t ch;
    while ((text = utf8codepoint(text, &ch)) && ch)
    {
        if (ch == 9)
            ch = 32;
        utf8_int32_t ch_next;
        utf8codepoint(text, &ch_next);
        if (ch < TEXT_CHARSET_START || ch > TEXT_CHARSET_END)
        {
            ch = 0x3f;
        }

        if (ch == '\n')
        {
            x_ = x;
            y_ += font->size;
            is_new_line = true;
            continue;
        }

        if (is_new_line && ch == ' ')
        {
            is_new_line = false;
            continue;
        }
        else if (is_new_line && ch != ' ')
        {
            is_new_line = false;
        }

        glyph g = font->glyphs[ch];

        s32 y__ = y_ + font->px_h + g.yoff;
        s32 x_to_render = x_ + (g.lsb);

        render_target rec = _get_actual_rect(x_to_render, y__, g.width, g.height);

        for (s32 y = rec.y; y < rec.h; y++)
        {
            for (s32 x = rec.x; x < rec.w; x++)
            {
                _copy_glyph_pixel(x, y, &g, rec, tint);
            }
        }

        /* add kerning */
        //kern = stbtt_GetCodepointKernAdvance(&font->info, ch, ch_next);
        x_ += (g.advance);

        if (x_ > x + cutoff_width)
        {
            x_ = x;
            y_ += font->size;
            is_new_line = true;
        }
    }

    return (y_ - y) + font->size;
}

static s32 cpu_calculate_cursor_position(font *font, char *text, s32 click_x)
{
    if (!font->loaded)
        return 0;

    s32 x = 0;
    s32 index = 0;
    utf8_int32_t ch;
    while ((text = utf8codepoint(text, &ch)) && ch)
    {
        if (ch == 9)
            ch = 32;
        utf8_int32_t ch_next;
        utf8codepoint(text, &ch_next);
        if (ch < TEXT_CHARSET_START || ch > TEXT_CHARSET_END)
        {
            ch = 0x3f;
        }
        if (ch == '\n')
            ch = 0xB6;

        glyph g = font->glyphs[ch];
        s32 width_next = font->glyphs[ch_next].width;

        /* add kerning */
        //kern = stbtt_GetCodepointKernAdvance(&font->info, ch, ch_next);
        x += (g.advance);
        if (x - (width_next / 5) > click_x)
        {
            return index;
        }

        ++index;
    }

    return index;
}

static s32 cpu_calculate_text_width_from_upto(font *font, char *text, s32 from, s32 index)
{
    if (!font->loaded)
        return 0;

    s32 x = 0;
    utf8_int32_t ch;
    s32 i = 0;
    while ((text = utf8codepoint(text, &ch)) && ch)
    {
        if (index == i)
            return x;

        if (ch == 9)
            ch = 32;
        utf8_int32_t ch_next;
        utf8codepoint(text, &ch_next);
        if (ch < TEXT_CHARSET_START || ch > TEXT_CHARSET_END)
        {
            ch = 0x3f;
        }
        if (ch == '\n')
            ch = 0xB6;

        glyph g = font->glyphs[ch];

        if (i >= from)
        {
            /* add kerning */
            //kern = stbtt_GetCodepointKernAdvance(&font->info, ch, ch_next);
            x += (g.advance);
        }

        i++;
    }

    return x;
}

static s32 cpu_calculate_text_width_upto(font *font, char *text, s32 index)
{
    if (!font->loaded)
        return 0;

    s32 x = 0;
    utf8_int32_t ch;
    s32 i = 0;
    while ((text = utf8codepoint(text, &ch)) && ch)
    {
        if (index == i)
            return x;

        if (ch == 9)
            ch = 32;
        utf8_int32_t ch_next;
        utf8codepoint(text, &ch_next);
        if (ch < TEXT_CHARSET_START || ch > TEXT_CHARSET_END)
        {
            ch = 0x3f;
        }
        if (ch == '\n')
            ch = 0xB6;

        glyph g = font->glyphs[ch];

        /* add kerning */
        //kern = stbtt_GetCodepointKernAdvance(&font->info, ch, ch_next);
        x += (g.advance);

        i++;
    }

    return x;
}

static s32 cpu_calculate_text_width(font *font, char *text)
{
    if (!font->loaded)
        return 0;

    float x = 0;
    utf8_int32_t ch;
    while ((text = utf8codepoint(text, &ch)) && ch)
    {
        if (ch == 9)
            ch = 32;
        utf8_int32_t ch_next;
        utf8codepoint(text, &ch_next);
        if (ch < TEXT_CHARSET_START || ch > TEXT_CHARSET_END)
        {
            ch = 0x3f;
        }
        if (ch == '\n')
            ch = 0xB6;

        glyph g = font->glyphs[ch];

        /* add kerning */
        //kern = stbtt_GetCodepointKernAdvance(&font->info, ch, ch_next);
        x += (g.advance);
    }

    return x;
}

static void cpu_render_triangle(s32 x, s32 y, s32 w, s32 h, color tint, triangle_direction dir)
{
    render_target rec = _get_actual_rect(x, y, w, h);

    s32 ac_w = rec.w - rec.x;

    if (dir == TRIANGLE_DOWN)
    {
        for (s32 y = rec.y; y < rec.h; y++)
        {
            s32 _y = y - rec.y;

            for (s32 x = rec.x; x < rec.w; x++)
            {
                s32 _x = x - rec.x;
                if (_x < _y / 2 || _x >= ac_w - (_y / 2) - 1)
                    continue;

                _set_pixel(x, y, tint);
            }
        }
    }
    else if (dir == TRIANGLE_UP)
    {
        s32 ac_h = rec.h - rec.y;

        for (s32 y = rec.y; y < rec.h; y++)
        {
            s32 _y = (y - rec.y);
            _y = ac_h - _y;

            for (s32 x = rec.x; x < rec.w; x++)
            {
                s32 _x = x - rec.x;
                if (_x < _y / 2 || _x >= ac_w - (_y / 2))
                    continue;

                _set_pixel(x, y, tint);
            }
        }
    }
    else if (dir == TRIANGLE_LEFT)
    {
        s32 ac_h = rec.h - rec.y;

        for (s32 y = rec.y; y < rec.h; y++)
        {
            s32 _y = (y - rec.y);
            _y = ac_h - _y;

            for (s32 x = rec.x; x < rec.w; x++)
            {
                s32 _x = x - rec.x;
                _x = ac_w - _x;
                if (_x / 2 > _y - 1 || _x / 2 >= ac_w - (_y))
                    continue;

                _set_pixel(x, y, tint);
            }
        }
    }
    else if (dir == TRIANGLE_RIGHT)
    {
        log_assert(0, "TRIANGLE_RIGHT not implemented");
    }
}

static void cpu_render_line(s32 x1, s32 y1, s32 x2, s32 y2, float thickness, color tint)
{
    log_assert(0, "render_line not implemented on cpu");
}

static void cpu_render_rectangle_outline(s32 x, s32 y, s32 width, s32 height, u16 outline_w, color tint)
{
    // left
    cpu_render_rectangle(x, y, outline_w, height, tint);
    // right
    cpu_render_rectangle(x + width - outline_w, y, outline_w, height, tint);
    // top
    cpu_render_rectangle(x + outline_w, y, width - (outline_w * 2), outline_w, tint);
    // bottom
    cpu_render_rectangle(x + outline_w, y + height - outline_w, width - (outline_w * 2), outline_w, tint);
}

static void cpu_render_set_scissor(platform_window *window, s32 x, s32 y, s32 w, s32 h)
{
    current_scissor = (vec4){x, y, w + 1, h + 1};
}

static vec4 cpu_render_get_scissor(platform_window *window)
{
    return current_scissor;
}

static s32 cpu_render_text_rd(font *font, s32 x, s32 y, char *text, color tint, u16 target_h)
{
	// TODO
	return 0;
}

static bool isInside(float circle_x, float circle_y,
                   float rad, int x, int y)
{
    // Compare radius of circle with distance
    // of its center from given point
    if ((x - circle_x) * (x - circle_x) +
        (y - circle_y) * (y - circle_y) <= rad * rad)
        return true;
    else
        return false;
}

static void cpu_render_rounded_rect(float x, float y, float width, float height, color tint, float radius, int innerPad)
{
	width -= innerPad;
	height -= innerPad;
	render_target rec = _get_actual_rect(x, y, width, height);

	radius = 2;
	float R = radius;
	s32 dx = 0;
	s32 dy = 0;

    for (s32 y = rec.y + innerPad; y < rec.h; y++)
    {
        for (s32 x = rec.x + innerPad; x < rec.w; x++)
        {
			s32 total_w = width - (innerPad);
			s32 total_h = height - (innerPad);

			if (dx > radius-1 && dx < total_w - radius)
			{
				_set_pixel(x, y, tint);
			}

			else if (dy > radius && dy < total_h - radius)
			{
				_set_pixel(x, y, tint);
			}

			// Top left circle
			else if (isInside(radius-1, radius, radius, dx, dy))
			{
				_set_pixel(x, y, tint);
			}

			// Bottom left circle
			else if (isInside(radius-1, total_h - radius - 1, radius, dx, dy))
			{
				_set_pixel(x, y, tint);
			}

			// Top right circle
			else if (isInside(total_w - radius - 1, radius, radius, dx, dy))
			{
				_set_pixel(x, y, tint);
			}

			// Bottom right circle
			else if (isInside(total_w - radius - 1, total_h - radius - 1, radius, dx, dy))
			{
				_set_pixel(x, y, tint);
			}

			dx++;
        }
		dx = 0;

		dy++;
    }
}

//
// Arc drawing
//
static void cpu_render_arc(float x1, float y1, float x2, float y2, float radius, bool arcDirection, bool useBiggerArc, color tint, float thickness)
{
    log_assert(0, "render_arc not implemented on cpu");
}

render_driver render_cpu_driver =
{
	"CPU",
	cpu_set_cpu_render_depth,
	cpu_render_clear,

	cpu_render_image,
	cpu_render_image_tint,
    
	cpu_render_text,
	cpu_render_text_ellipsed,
	cpu_render_text_cutoff,
	cpu_render_text_with_cursor,
	cpu_render_text_with_selection,

	cpu_calculate_cursor_position,
	cpu_calculate_text_width,
	cpu_calculate_text_width_upto,
	cpu_calculate_text_width_from_upto,

	cpu_render_rectangle,
	cpu_render_line,
	cpu_render_rectangle_outline,
	cpu_render_triangle,

	cpu_render_set_scissor,
	cpu_render_get_scissor,
	cpu_render_reset_scissor,

	cpu_render_set_rotation,
	cpu_render_reset_rotation,
	cpu_render_arc,

	cpu_render_text_rd,
	cpu_render_rounded_rect,
};