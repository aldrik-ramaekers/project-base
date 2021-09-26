/* 
*  BSD 2-Clause “Simplified” License
*  Copyright (c) 2019, Aldrik Ramaekers, aldrik.ramaekers@protonmail.com
*  All rights reserved.
*/

static u8 gl_render_depth = 1;
static vec4 current_scissor;
static float global_rotation = 0.0f;

static void gl_render_clear(platform_window *window, color tint)
{
    IMP_glClearColor(tint.r / 255.0f, tint.g / 255.0f, tint.b / 255.0f, tint.a / 255.0f);
    IMP_glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

static void gl_render_set_rotation(float32 rotation)
{
    global_rotation = rotation;
}

static void gl_render_reset_rotation(float32 rotation, float32 x, float32 y)
{
    IMP_glPopMatrix();
}

static void gl_set_gl_render_depth(s32 depth)
{
    gl_render_depth = depth;
}

static vec2f gl_rotateUV(vec2f uv, float rotation)
{
	uv = (vec2f){uv.x - 0.5f, uv.y - 0.5f};
    float m2x2[4] = { cos(rotation), sin(rotation), -sin(rotation), cos(rotation) };

	vec2f result = {0,0};
#if 0
	result.x = m2x2[0] * uv.x + m2x2[1] * uv.y;
	result.y = m2x2[2] * uv.x + m2x2[3] * uv.y;
#else
	result.x = m2x2[0] * uv.x + m2x2[2] * uv.y;
	result.y = m2x2[1] * uv.x + m2x2[3] * uv.y;
#endif
	
    result = (vec2f){result.x + 0.5f, result.y + 0.5f};
	return result;
}

static void gl_render_rectangle(s32 x, s32 y, s32 width, s32 height, color tint)
{
    IMP_glBindTexture(GL_TEXTURE_2D, 0);
    IMP_glBegin(GL_QUADS);
    IMP_glColor4f(tint.r / 255.0f, tint.g / 255.0f, tint.b / 255.0f, tint.a / 255.0f);
    IMP_glVertex3i(x, y, gl_render_depth);
    IMP_glVertex3i(x, y + height, gl_render_depth);
    IMP_glVertex3i(x + width, y + height, gl_render_depth);
    IMP_glVertex3i(x + width, y, gl_render_depth);
    IMP_glEnd();
}

static void gl_render_image(image *image, s32 x, s32 y, s32 width, s32 height)
{
    log_assert(image, "Image cannot be null");

    if (image->loaded)
    {
        IMP_glBindTexture(GL_TEXTURE_2D, image->textureID);
        IMP_glEnable(GL_TEXTURE_2D);
        IMP_glBegin(GL_QUADS);
        IMP_glColor4f(1., 1., 1., 1.);

        vec2f topleft = gl_rotateUV((vec2f){0, 0}, global_rotation);
        vec2f bottomleft = gl_rotateUV((vec2f){0, 1}, global_rotation);
        vec2f bottomright = gl_rotateUV((vec2f){1, 1}, global_rotation);
        vec2f topright = gl_rotateUV((vec2f){1, 0}, global_rotation);

        IMP_glPushMatrix();
        IMP_glTranslatef(x + (width / 2), y + (height / 2), 0);

        IMP_glTexCoord2f(topleft.x, topleft.y);
        IMP_glVertex3i(x, y, gl_render_depth);
        IMP_glTexCoord2f(bottomleft.x, bottomleft.y);
        IMP_glVertex3i(x, y + height, gl_render_depth);
        IMP_glTexCoord2f(bottomright.x, bottomright.y);
        IMP_glVertex3i(x + width, y + height, gl_render_depth);
        IMP_glTexCoord2f(topright.x, topright.y);
        IMP_glVertex3i(x + width, y, gl_render_depth);

        IMP_glPopMatrix();

        IMP_glEnd();
        IMP_glBindTexture(GL_TEXTURE_2D, 0);
        IMP_glDisable(GL_TEXTURE_2D);
    }
}

static void gl_render_image_tint(image *image, s32 x, s32 y, s32 width, s32 height, color tint)
{
    log_assert(image, "Image cannot be null");

    if (image->loaded)
    {
        IMP_glBindTexture(GL_TEXTURE_2D, image->textureID);
        IMP_glEnable(GL_TEXTURE_2D);
        IMP_glBegin(GL_QUADS);
        IMP_glColor4f(tint.r / 255.0f, tint.g / 255.0f, tint.b / 255.0f, tint.a / 255.0f);

		vec2f topleft = gl_rotateUV((vec2f){0, 0}, global_rotation);
        vec2f bottomleft = gl_rotateUV((vec2f){0, 1}, global_rotation);
        vec2f bottomright = gl_rotateUV((vec2f){1, 1}, global_rotation);
        vec2f topright = gl_rotateUV((vec2f){1, 0}, global_rotation);

		IMP_glPushMatrix();
        IMP_glTranslatef(x + (width / 2), y + (height / 2), 0);
		
        IMP_glTexCoord2f(topleft.x, topleft.y);
        IMP_glVertex3i(x, y, gl_render_depth);
        IMP_glTexCoord2f(bottomleft.x, bottomleft.y);
        IMP_glVertex3i(x, y + height, gl_render_depth);
        IMP_glTexCoord2f(bottomright.x, bottomright.y);
        IMP_glVertex3i(x + width, y + height, gl_render_depth);
        IMP_glTexCoord2f(topright.x, topright.y);
        IMP_glVertex3i(x + width, y, gl_render_depth);

        IMP_glPopMatrix();

        IMP_glEnd();
        IMP_glBindTexture(GL_TEXTURE_2D, 0);
        IMP_glDisable(GL_TEXTURE_2D);
    }
}

static s32 gl_render_text_ellipsed(font *font, s32 x, s32 y, s32 maxw, char *text, color tint)
{
    if (!font->loaded)
        return 0;

    IMP_glEnable(GL_TEXTURE_2D);
    IMP_glColor4f(tint.r / 255.0f, tint.g / 255.0f, tint.b / 255.0f, tint.a / 255.0f);

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

        IMP_glBindTexture(GL_TEXTURE_2D, g.textureID);
        IMP_glBegin(GL_QUADS);

        IMP_glTexCoord2i(0, 0);
        IMP_glVertex3i(x_to_render, y_, gl_render_depth);
        IMP_glTexCoord2i(0, 1);
        IMP_glVertex3i(x_to_render, y_ + g.height, gl_render_depth);
        IMP_glTexCoord2i(1, 1);
        IMP_glVertex3i(x_to_render + g.width, y_ + g.height, gl_render_depth);
        IMP_glTexCoord2i(1, 0);
        IMP_glVertex3i(x_to_render + g.width, y_, gl_render_depth);
        IMP_glBindTexture(GL_TEXTURE_2D, 0);
        IMP_glEnd();

        /* add kerning */
        //kern = stbtt_GetCodepointKernAdvance(&font->info, ch, ch_next);
        x_ += (g.advance);

        if (!in_ellipse && (x_ - x) > maxw - (font->glyphs['.'].width * 3) && count < len - 3)
        {
            in_ellipse = true;
            text = ellipse;
        }
    }

    IMP_glDisable(GL_TEXTURE_2D);

    return maxw;
}

static s32 gl_render_text_with_selection(font *font, s32 x, s32 y, char *text, color tint, s32 selection_start, s32 selection_length)
{
    if (!font->loaded)
        return 0;

    IMP_glEnable(GL_TEXTURE_2D);
    IMP_glColor4f(tint.r / 255.0f, tint.g / 255.0f, tint.b / 255.0f, tint.a / 255.0f);

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

        IMP_glBindTexture(GL_TEXTURE_2D, g.textureID);
        IMP_glBegin(GL_QUADS);

        IMP_glTexCoord2i(0, 0);
        IMP_glVertex3i(x_to_render, y_, gl_render_depth);
        IMP_glTexCoord2i(0, 1);
        IMP_glVertex3i(x_to_render, y_ + g.height, gl_render_depth);
        IMP_glTexCoord2i(1, 1);
        IMP_glVertex3i(x_to_render + g.width, y_ + g.height, gl_render_depth);
        IMP_glTexCoord2i(1, 0);
        IMP_glVertex3i(x_to_render + g.width, y_, gl_render_depth);
        IMP_glBindTexture(GL_TEXTURE_2D, 0);
        IMP_glEnd();

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

    IMP_glDisable(GL_TEXTURE_2D);

    gl_render_rectangle(selection_start_x, y - 3, selection_end_x - selection_start_x, TEXTBOX_HEIGHT - 10, rgba(66, 134, 244, 120));

    return x_ - x;
}

static s32 gl_render_text_with_cursor(font *font, s32 x, s32 y, char *text, color tint, s32 cursor_pos)
{
    if (!font->loaded)
        return 0;

    IMP_glEnable(GL_TEXTURE_2D);
    IMP_glColor4f(tint.r / 255.0f, tint.g / 255.0f, tint.b / 255.0f, tint.a / 255.0f);

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

        IMP_glBindTexture(GL_TEXTURE_2D, g.textureID);
        IMP_glBegin(GL_QUADS);

        IMP_glTexCoord2i(0, 0);
        IMP_glVertex3i(x_to_render, y_, gl_render_depth);
        IMP_glTexCoord2i(0, 1);
        IMP_glVertex3i(x_to_render, y_ + g.height, gl_render_depth);
        IMP_glTexCoord2i(1, 1);
        IMP_glVertex3i(x_to_render + g.width, y_ + g.height, gl_render_depth);
        IMP_glTexCoord2i(1, 0);
        IMP_glVertex3i(x_to_render + g.width, y_, gl_render_depth);
        IMP_glBindTexture(GL_TEXTURE_2D, 0);
        IMP_glEnd();

        /* add kerning */
        //kern = stbtt_GetCodepointKernAdvance(&font->info, ch, ch_next);
        x_ += (g.advance);
        index++;

        if (index == cursor_pos)
        {
            cursor_x = x_;
        }
    }

    IMP_glDisable(GL_TEXTURE_2D);

    gl_render_rectangle(cursor_x, y - 3, 2, TEXTBOX_HEIGHT - 10, global_ui_context.style.textbox_foreground);

    return x_ - x;
}

static s32 gl_render_text_rd(font *font, s32 x, s32 y, char *text, color tint, u16 target_h)
{
	float size_multiplier = target_h/(float)font->size;

	if (!font->loaded)
        return 0;

    IMP_glEnable(GL_TEXTURE_2D);
    IMP_glColor4f(tint.r / 255.0f, tint.g / 255.0f, tint.b / 255.0f, tint.a / 255.0f);

    s32 x_ = x;
    utf8_int32_t ch;
    while ((text = utf8codepoint(text, &ch)) && ch)
    {
        if (ch == 9) ch = 32; // Turn tab into space.
        if (ch < TEXT_CHARSET_START || ch > TEXT_CHARSET_END) ch = 0x3f; // Default character for out of range glyphs.
        if (ch == 10) ch = 0xB6; // What the hell is this?
        if (ch == 13) continue; // Don't draw newlines.

        glyph g = font->glyphs[ch];

        s32 y_ = y + (font->px_h + g.yoff)*size_multiplier;
        s32 x_to_render = x_ + (g.lsb*size_multiplier);

        IMP_glBindTexture(GL_TEXTURE_2D, g.textureID);
        IMP_glBegin(GL_QUADS);

        IMP_glTexCoord2i(0, 0);
        IMP_glVertex3i(x_to_render, y_, gl_render_depth);
        IMP_glTexCoord2i(0, 1);
        IMP_glVertex3i(x_to_render, y_ + g.height*size_multiplier, gl_render_depth);
        IMP_glTexCoord2i(1, 1);
        IMP_glVertex3i(x_to_render + g.width*size_multiplier, y_ + g.height*size_multiplier, gl_render_depth);
        IMP_glTexCoord2i(1, 0);
        IMP_glVertex3i(x_to_render + g.width*size_multiplier, y_, gl_render_depth);
        IMP_glBindTexture(GL_TEXTURE_2D, 0);
        IMP_glEnd();

        x_ += (g.advance*size_multiplier);
    }

    IMP_glDisable(GL_TEXTURE_2D);

    return x_ - x;
}

static s32 gl_render_text(font *font, s32 x, s32 y, char *text, color tint)
{
    if (!font->loaded)
        return 0;

    IMP_glEnable(GL_TEXTURE_2D);
    IMP_glColor4f(tint.r / 255.0f, tint.g / 255.0f, tint.b / 255.0f, tint.a / 255.0f);

    s32 x_ = x;
    utf8_int32_t ch;
    while ((text = utf8codepoint(text, &ch)) && ch)
    {
        if (ch == 9) ch = 32; // Turn tab into space.
        if (ch < TEXT_CHARSET_START || ch > TEXT_CHARSET_END) ch = 0x3f; // Default character for out of range glyphs.
        if (ch == 10) ch = 0xB6; // What the hell is this?
        if (ch == 13) continue; // Don't draw newlines.

        glyph g = font->glyphs[ch];

        s32 y_ = y + font->px_h + g.yoff;
        s32 x_to_render = x_ + (g.lsb);

        IMP_glBindTexture(GL_TEXTURE_2D, g.textureID);
        IMP_glBegin(GL_QUADS);

        IMP_glTexCoord2i(0, 0);
        IMP_glVertex3i(x_to_render, y_, gl_render_depth);
        IMP_glTexCoord2i(0, 1);
        IMP_glVertex3i(x_to_render, y_ + g.height, gl_render_depth);
        IMP_glTexCoord2i(1, 1);
        IMP_glVertex3i(x_to_render + g.width, y_ + g.height, gl_render_depth);
        IMP_glTexCoord2i(1, 0);
        IMP_glVertex3i(x_to_render + g.width, y_, gl_render_depth);
        IMP_glBindTexture(GL_TEXTURE_2D, 0);
        IMP_glEnd();

        /* add kerning */
		//utf8_int32_t ch_next;
        //utf8codepoint(text, &ch_next);
        //kern = stbtt_GetCodepointKernAdvance(&font->info, ch, ch_next);
        x_ += (g.advance);
    }

    IMP_glDisable(GL_TEXTURE_2D);

    return x_ - x;
}

static s32 gl_render_text_cutoff(font *font, s32 x, s32 y, char *text, color tint, u16 cutoff_width)
{
    if (!font->loaded)
        return 0;

    IMP_glEnable(GL_TEXTURE_2D);
    IMP_glColor4f(tint.r / 255.0f, tint.g / 255.0f, tint.b / 255.0f, tint.a / 255.0f);

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

		if (x_to_render+g.advance > x + cutoff_width)
        {
            x_ = x;
            y_ += font->size;
			y__ = y_ + font->px_h + g.yoff;
            x_to_render = x_;
        }

        IMP_glBindTexture(GL_TEXTURE_2D, g.textureID);
        IMP_glBegin(GL_QUADS);

        IMP_glTexCoord2i(0, 0);
        IMP_glVertex3i(x_to_render, y__, gl_render_depth);
        IMP_glTexCoord2i(0, 1);
        IMP_glVertex3i(x_to_render, y__ + g.height, gl_render_depth);
        IMP_glTexCoord2i(1, 1);
        IMP_glVertex3i(x_to_render + g.width, y__ + g.height, gl_render_depth);
        IMP_glTexCoord2i(1, 0);
        IMP_glVertex3i(x_to_render + g.width, y__, gl_render_depth);
        IMP_glBindTexture(GL_TEXTURE_2D, 0);
        IMP_glEnd();

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

    IMP_glDisable(GL_TEXTURE_2D);

    return (y_ - y) + font->size;
}

static s32 gl_calculate_cursor_position(font *font, char *text, s32 click_x)
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

static s32 gl_calculate_text_width_from_upto(font *font, char *text, s32 from, s32 index)
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

static s32 gl_calculate_text_width_upto(font *font, char *text, s32 index)
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

static s32 gl_calculate_text_width(font *font, char *text)
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

static void gl_render_triangle(s32 x, s32 y, s32 w, s32 h, color tint, triangle_direction dir)
{
    IMP_glBegin(GL_TRIANGLES);
    IMP_glColor4f(tint.r / 255.0f, tint.g / 255.0f, tint.b / 255.0f, tint.a / 255.0f);

    if (dir == TRIANGLE_DOWN)
    {
        IMP_glVertex3i(x + (w / 2), y + h, gl_render_depth);
        IMP_glVertex3i(x, y, gl_render_depth);
        IMP_glVertex3i(x + w, y, gl_render_depth);
    }
    else if (dir == TRIANGLE_UP)
    {
        IMP_glVertex3i(x + (w / 2), y, gl_render_depth);
        IMP_glVertex3i(x + w, y + h, gl_render_depth);
        IMP_glVertex3i(x, y + h, gl_render_depth);
    }
    else if (dir == TRIANGLE_LEFT)
    {
        IMP_glVertex3i(x, y + (w / 2), gl_render_depth);
        IMP_glVertex3i(x + h, y, gl_render_depth);
        IMP_glVertex3i(x + h, y + w, gl_render_depth);
    }
    else if (dir == TRIANGLE_RIGHT)
    {
        log_assert(0, "TRIANGLE_RIGHT not implemented");
    }

    IMP_glEnd();
}

static void gl_render_line(s32 x1, s32 y1, s32 x2, s32 y2, float thickness, color tint)
{
    IMP_glBindTexture(GL_TEXTURE_2D, 0);
    IMP_glLineWidth(thickness);
    IMP_glColor4f(tint.r / 255.0f, tint.g / 255.0f, tint.b / 255.0f, tint.a / 255.0f);
    IMP_glBegin(GL_LINES);
    IMP_glVertex3i(x1, y1, gl_render_depth);
    IMP_glVertex3i(x2, y2, gl_render_depth);
    IMP_glEnd();
}

static void gl_render_rectangle_outline(s32 x, s32 y, s32 width, s32 height, u16 outline_w, color tint)
{
    // left
    gl_render_rectangle(x, y, outline_w, height, tint);
    // right
    gl_render_rectangle(x + width - outline_w, y, outline_w, height, tint);
    // top
    gl_render_rectangle(x + outline_w, y, width - (outline_w * 2), outline_w, tint);
    // bottom
    gl_render_rectangle(x + outline_w, y + height - outline_w, width - (outline_w * 2), outline_w, tint);
}

static void gl_render_set_scissor(platform_window *window, s32 x, s32 y, s32 w, s32 h)
{
    IMP_glEnable(GL_SCISSOR_TEST);
	if (window) IMP_glScissor(x - 1, window->height - h - y - 1, w + 1, h + 1);
	else IMP_glScissor(x - 1, y - 1 + h, w + 1, h + 1);
}

static vec4 gl_render_get_scissor()
{
	GLboolean enabled = false;
	IMP_glGetBooleanv(GL_SCISSOR_TEST,&enabled);
    vec4 vec;

	if (enabled) {
		IMP_glGetIntegerv(GL_SCISSOR_BOX, (GLint *)(&vec));
		vec.x += 1;
    	vec.w -= 1;
		vec.h -= 1;
		vec.y += 1;
	} else {
		vec = (vec4){0,0,99999,99999};
	}
    return vec;
}

static void gl_render_reset_scissor()
{
    IMP_glDisable(GL_SCISSOR_TEST);
}

//
// Arc drawing
//
static float gl_normalizeAngleToSmallestPositive(float angle)
{
    while (angle < 0.0)
    {
        angle += M_PI * 2;
    }
    while (angle >= M_PI * 2)
    {
        angle -= M_PI * 2;
    }
    return angle;
}

const int ARC_VERTEX_COUNT = 100;
static void gl__render_arc(float angle1, float angle2, float radius, float x, float y, float useBiggerArc, color tint, float thickness)
{
    // Prepare angles
    angle1 = gl_normalizeAngleToSmallestPositive(angle1);
    angle2 = gl_normalizeAngleToSmallestPositive(angle2);
    if (angle1 > angle2)
    {
        float buffer = angle1;
        angle1 = angle2;
        angle2 = buffer;
    }
    if (useBiggerArc != (angle2 - angle1 > M_PI))
    {
        angle1 += M_PI * 2;
    }

    s32 buffer_size = ARC_VERTEX_COUNT * 2;

    // Create opengl geometry
    GLfloat pos[200];
    for (int i = 0; i < ARC_VERTEX_COUNT; i++)
    {
        pos[i * 2] = sin((float)i / (ARC_VERTEX_COUNT - 1) * (angle2 - angle1) + angle1) * radius + x;
        pos[i * 2 + 1] = cos((float)i / (ARC_VERTEX_COUNT - 1) * (angle2 - angle1) + angle1) * radius + y;
    }

    IMP_glBindTexture(GL_TEXTURE_2D, 0);
    IMP_glBegin(GL_LINE_STRIP);
    IMP_glLineWidth(thickness);
    IMP_glColor4f(tint.r / 255.0f, tint.g / 255.0f, tint.b / 255.0f, tint.a / 255.0f);
    for (int i = 0; i < buffer_size; i += 2)
    {
        //printf("pos: %f %f, --- x: %f y: %f\n", x, y, pos[i], pos[i+1]);
        IMP_glVertex3i(pos[i], pos[i + 1], gl_render_depth);
    }
    IMP_glEnd();
}

static void gl_render_arc(float x1, float y1, float x2, float y2, float radius, bool arcDirection, bool useBiggerArc, color tint, float thickness)
{
    // distance between points
    float distance = sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
    // halfway point
    float xAverage = (x1 + x2) / 2.0;
    float yAverage = (y1 + y2) / 2.0;
    // circle center
    float xCenter = sqrt(radius * radius - distance * distance / 4.0) * (y1 - y2) / distance;
    float yCenter = sqrt(radius * radius - distance * distance / 4.0) * (x2 - x1) / distance;
    xCenter = xAverage + (arcDirection ? xCenter : -xCenter);
    yCenter = yAverage + (arcDirection ? yCenter : -yCenter);
    // angles
    float angle1 = atan2(x1 - xCenter, y1 - yCenter);
    float angle2 = atan2(x2 - xCenter, y2 - yCenter);
    // create the arc

    //printf("%f %f %f, %f\n", distance, xCenter, yCenter, (radius*radius - distance*distance/4.0));
    gl__render_arc(angle1, angle2, radius, xCenter, yCenter, useBiggerArc, tint, thickness);
}

#define ROUNDING_POINT_COUNT 16      // Larger values makes circle smoother.
void gl_render_rounded_rect(float x, float y, float width, float height, color tint, float radius, int innerPad)
{
	y += height;
    vec2f top_left[ROUNDING_POINT_COUNT];
    vec2f bottom_left[ROUNDING_POINT_COUNT];
    vec2f top_right[ROUNDING_POINT_COUNT];
    vec2f bottom_right[ROUNDING_POINT_COUNT];

    if( radius == 0.0 )
    {
        radius = min(width, height);
        radius *= 0.10; // 10%
    }

    int i = 0;
    float x_offset, y_offset;
    float step = ( 2.0f * M_PI ) / (ROUNDING_POINT_COUNT * 4),
          angle = 0.0f;

    unsigned int index = 0, segment_count = ROUNDING_POINT_COUNT;
    vec2f bottom_left_corner = { x + radius, y - height + radius }; 


    while( i != segment_count )
    {
        x_offset = cosf( angle );
        y_offset = sinf( angle );


        top_left[ index ].x = bottom_left_corner.x - 
                              ( x_offset * radius );
        top_left[ index ].y = ( height - ( radius * 2.0f ) ) + 
                                bottom_left_corner.y - 
                              ( y_offset * radius );


        top_right[ index ].x = ( width - ( radius * 2.0f ) ) + 
                                 bottom_left_corner.x + 
                               ( x_offset * radius ) + (i == 0 ? -1 : 0);
        top_right[ index ].y = ( height - ( radius * 2.0f ) ) + 
                                 bottom_left_corner.y -
                               ( y_offset * radius );


        bottom_right[ index ].x = ( width - ( radius * 2.0f ) ) +
                                    bottom_left_corner.x + 
                                  ( x_offset * radius ) + (i == 0 ? -1 : 0);
        bottom_right[ index ].y = bottom_left_corner.y + 
                                  ( y_offset * radius );


        bottom_left[ index ].x = bottom_left_corner.x - 
                                 ( x_offset * radius );
        bottom_left[ index ].y = bottom_left_corner.y +
                                 ( y_offset * radius );

#if 1
        top_left[ index ].x = (top_left[ index ].x)+innerPad;
        top_left[ index ].y = (top_left[ index ].y)-innerPad;


        top_right[ index ].x = (top_right[ index ].x)-innerPad;
        top_right[ index ].y = (top_right[ index ].y)-innerPad;


        bottom_right[ index ].x = (bottom_right[ index ].x)-innerPad;
        bottom_right[ index ].y = (bottom_right[ index ].y)+innerPad;


        bottom_left[ index ].x =  (bottom_left[ index ].x)+innerPad;
        bottom_left[ index ].y =  (bottom_left[ index ].y)+innerPad;
#endif

        angle -= step;

        ++index;

        ++i;
    }
	
	IMP_glBindTexture(GL_TEXTURE_2D, 0);
    IMP_glColor4f(tint.r / 255.0f, tint.g / 255.0f, tint.b / 255.0f, tint.a / 255.0f);
    IMP_glBegin( GL_TRIANGLE_STRIP );
    {
        // Top
        for( i = segment_count - 1 ; i >= 0 ; i--)
        {
            IMP_glVertex3i( top_left[ i ].x, top_left[ i ].y, gl_render_depth);
            IMP_glVertex3i( top_right[ i ].x, top_right[ i ].y, gl_render_depth );
        }

        // In order to stop and restart the strip.
        IMP_glVertex3i( top_right[ 0 ].x, top_right[ 0 ].y, gl_render_depth );
        IMP_glVertex3i( top_right[ 0 ].x, top_right[ 0 ].y, gl_render_depth );

        // Center
        IMP_glVertex3i( top_right[ 0 ].x, top_right[ 0 ].y, gl_render_depth );
        IMP_glVertex3i( top_left[ 0 ].x, top_left[ 0 ].y, gl_render_depth );
        IMP_glVertex3i( bottom_right[ 0 ].x, bottom_right[ 0 ].y, gl_render_depth );
        IMP_glVertex3i( bottom_left[ 0 ].x, bottom_left[ 0 ].y, gl_render_depth );

        // Bottom
        for( i = 0; i != segment_count ; i++ )
        {
            IMP_glVertex3i( bottom_right[ i ].x, bottom_right[ i ].y, gl_render_depth );    
            IMP_glVertex3i( bottom_left[ i ].x, bottom_left[ i ].y, gl_render_depth );                                    
        }    
    }
    IMP_glEnd();
}

render_driver render_gl_driver =
{
	"GL",
	gl_set_gl_render_depth,
	gl_render_clear,

	gl_render_image,
	gl_render_image_tint,
    
	gl_render_text,
	gl_render_text_ellipsed,
	gl_render_text_cutoff,
	gl_render_text_with_cursor,
	gl_render_text_with_selection,

	gl_calculate_cursor_position,
	gl_calculate_text_width,
	gl_calculate_text_width_upto,
	gl_calculate_text_width_from_upto,

	gl_render_rectangle,
	gl_render_line,
	gl_render_rectangle_outline,
	gl_render_triangle,

	gl_render_set_scissor,
	gl_render_get_scissor,
	gl_render_reset_scissor,

	gl_render_set_rotation,
	gl_render_reset_rotation,
	gl_render_arc,

	gl_render_text_rd,
	gl_render_rounded_rect,
};