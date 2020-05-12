/* 
*  BSD 2-Clause “Simplified” License
*  Copyright (c) 2019, Aldrik Ramaekers, aldrik.ramaekers@protonmail.com
*  All rights reserved.
*/

void assets_create()
{
	assets asset_collection;
	asset_collection.images = array_create(sizeof(image));
	asset_collection.fonts = array_create(sizeof(font));
	
	array_reserve(&asset_collection.images, ASSET_IMAGE_COUNT);
	array_reserve(&asset_collection.fonts, ASSET_FONT_COUNT);
	
	asset_collection.queue.queue = array_create(sizeof(asset_task));
	asset_collection.post_process_queue = array_create(sizeof(asset_task));
	
	array_reserve(&asset_collection.queue.queue, ASSET_QUEUE_COUNT);
	array_reserve(&asset_collection.post_process_queue, ASSET_QUEUE_COUNT);
	
	asset_mutex = mutex_create();
	asset_collection.valid = true;
	asset_collection.done_loading_assets = false;
	
	global_asset_collection = asset_collection;
}

inline static bool is_big_endian()
{
	volatile uint32_t i=0x01234567;
    // return 1 for big endian, 0 for little endian.
    return !((*((uint8_t*)(&i))) == 0x67);
}

bool assets_do_post_process()
{
	bool result = false;
#if 0
	static PFNGLTEXIMAGE2DMULTISAMPLEPROC glTexImage2DMultisample = NULL;
#ifdef OS_WIN
	if (!glTexImage2DMultisample)
		glTexImage2DMultisample = (PFNGLTEXIMAGE2DMULTISAMPLEPROC)wglGetProcAddress("glTexImage2DMultisample");
#endif
#endif
	
	mutex_lock(&asset_mutex);
	
	for (int i = 0; i < global_asset_collection.post_process_queue.length; i++)
	{
		asset_task *task = array_at(&global_asset_collection.post_process_queue, i);
		
		if (task->type == ASSET_IMAGE || task->type == ASSET_BITMAP)
		{
			if (task->image->data && task->valid)
			{
				glGenTextures(1, &task->image->textureID);
				glBindTexture(GL_TEXTURE_2D, task->image->textureID);
				
				s32 flag = is_big_endian() ? GL_UNSIGNED_INT_8_8_8_8 : 
				GL_UNSIGNED_INT_8_8_8_8_REV;
				
				glTexImage2D(GL_TEXTURE_2D, 0,GL_RGBA8, task->image->width, 
							 task->image->height, 0,  GL_RGBA, flag, task->image->data);
				
#if 0
				if (glTexImage2DMultisample)
					glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGBA8, task->image->width, task->image->height, FALSE);
#endif
				
#if 0
				s32 fbo;
				glGenFramebuffers(1, &fbo);
				glBindFramebuffer(GL_FRAMEBUFFER, fbo);
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, task->image->textureID, 0);
#endif
				
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				task->image->loaded = true;
				glBindTexture(GL_TEXTURE_2D, 0);
				
				if (!task->image->keep_in_memory)
					stbi_image_free(task->image->data);
			}
		}
		else if (task->type == ASSET_FONT)
		{
			if (task->valid)
			{
				for (s32 i = TEXT_CHARSET_START; i < TEXT_CHARSET_END; i++)
				{
					glyph *g = &task->font->glyphs[i];
					
					glGenTextures(1, &g->textureID);
					glBindTexture(GL_TEXTURE_2D, g->textureID);
					
					glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
					glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
                    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
                    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
					glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
					glTexImage2D( GL_TEXTURE_2D, 0, GL_ALPHA, g->width,g->height,
								 0, GL_ALPHA, GL_UNSIGNED_BYTE, g->bitmap );
					
					mem_free(g->bitmap);
				}
				
				task->font->loaded = true;
			}
		}
		
		result = true;
		
		array_remove_at(&global_asset_collection.post_process_queue, i);
	}
	
	mutex_unlock(&asset_mutex);
	
	return result;
}

bool assets_queue_worker_load_bitmap(image *image)
{
#ifdef MODE_DEVELOPER
	u64 stamp = platform_get_time(TIME_FULL, TIME_US);
#endif
	
	image->data = image->start_addr;
	
	debug_print_elapsed(stamp, "loaded image in");
	
	return !(image->data == 0);
}

bool assets_queue_worker_load_image(image *image)
{
#ifdef MODE_DEVELOPER
	u64 stamp = platform_get_time(TIME_FULL, TIME_US);
#endif
	
	set_active_directory(binary_path);
	
	image->data = stbi_load_from_memory(image->start_addr,
										image->end_addr - image->start_addr,
										&image->width,
										&image->height,
										&image->channels,
										STBI_rgb_alpha);
	
	debug_print_elapsed(stamp, "loaded image in");
	
	return !(image->data == 0);
}

bool assets_queue_worker_load_font(font *font)
{
#ifdef MODE_DEVELOPER
	u64 stamp = platform_get_time(TIME_FULL, TIME_US);
#endif
	
	unsigned char *ttf_buffer = (unsigned char*)font->start_addr;
	
    stbtt_fontinfo info;
    if (!stbtt_InitFont(&info, ttf_buffer, stbtt_GetFontOffsetForIndex(ttf_buffer,0)))
    {
		return false;
	}
	float scale = stbtt_ScaleForPixelHeight(&info, font->size);
	
	for (s32 i = TEXT_CHARSET_START; i < TEXT_CHARSET_END; i++)
	{
		s32 w, h, xoff, yoff;
		
		glyph new_glyph;
		new_glyph.bitmap = stbtt_GetCodepointBitmap(&info, 0, scale, i, &w, &h, &xoff, &yoff);
		new_glyph.width = w;
		new_glyph.height = h;
		new_glyph.xoff = xoff;
		new_glyph.yoff = yoff;
		
		stbtt_GetCodepointHMetrics(&info, i, &new_glyph.advance, &new_glyph.lsb);
		new_glyph.advance *= scale;
		new_glyph.lsb *= scale;
		
		if (i == 'M') font->px_h = -yoff;
		
		font->glyphs[i-TEXT_CHARSET_START] = new_glyph;
	}
	
	font->info = info;
	font->scale = scale;
	
	debug_print_elapsed(stamp, "loaded font in");
	
	return true;
}

void *assets_queue_worker()
{
	while (global_asset_collection.valid && !global_asset_collection.done_loading_assets)
	{
		if (mutex_trylock(&asset_mutex))
		{
			int queue_length = global_asset_collection.queue.queue.length;
			if (!queue_length) 
			{
				mutex_unlock(&asset_mutex);
				continue;
			}
			
			asset_task *task = array_at(&global_asset_collection.queue.queue, 0);
			asset_task buf = *task;
			array_remove_at(&global_asset_collection.queue.queue, 0);
			mutex_unlock(&asset_mutex);
			
			// load here
			if (buf.type == ASSET_IMAGE)
			{
				bool result = assets_queue_worker_load_image(buf.image);
				buf.valid = result;
			}
			if (buf.type == ASSET_BITMAP)
			{
				bool result = assets_queue_worker_load_bitmap(buf.image);
				buf.valid = result;
			}
			else if (buf.type == ASSET_FONT)
			{
				bool result = assets_queue_worker_load_font(buf.font);
				buf.valid = result;
			}
			
			mutex_lock(&asset_mutex);
			
			assert(global_asset_collection.post_process_queue.reserved_length > 
				   global_asset_collection.post_process_queue.length);
			
			array_push(&global_asset_collection.post_process_queue, &buf);
			mutex_unlock(&asset_mutex);
		}
		
		thread_sleep(1000);
	}
	
	thread_exit();
	
	return 0;
}

image *assets_load_image(u8 *start_addr, u8 *end_addr, bool keep_in_memory)
{
	// check if image is already loaded or loading
	for (int i = 0; i < global_asset_collection.images.length; i++)
	{
		image *img_at = array_at(&global_asset_collection.images, i);
		
		if (start_addr == img_at->start_addr)
		{
			// image is already loaded/loading
			img_at->references++;
			return img_at;
		}
	}
	
	image new_image;
	new_image.loaded = false;
	new_image.start_addr = start_addr;
	new_image.end_addr = end_addr;
	new_image.references = 1;
	new_image.keep_in_memory = keep_in_memory;
	
	// NOTE(Aldrik): we should never realloc the image array because pointers will be 
	// invalidated.
	assert(global_asset_collection.images.reserved_length > global_asset_collection.images.length);
	
	int index = array_push(&global_asset_collection.images, &new_image);
	
	asset_task task;
	task.type = ASSET_IMAGE;
	task.image = array_at(&global_asset_collection.images, index);
	
	mutex_lock(&asset_mutex);
	array_push(&global_asset_collection.queue.queue, &task);
	mutex_unlock(&asset_mutex);
	
	return task.image;
}

void assets_destroy_image(image *image_to_destroy)
{
	if (image_to_destroy->references == 1)
	{
		glBindTexture(GL_TEXTURE_2D, 0);
		glDeleteTextures(1, &image_to_destroy->textureID);
		
		if (image_to_destroy->keep_in_memory)
			stbi_image_free(image_to_destroy->data);
		
		//array_remove(&global_asset_collection.images, image_at);
	}
	else
	{
		image_to_destroy->references--;
	}
}

font *assets_load_font(u8 *start_addr, u8 *end_addr, s16 size)
{
	//assert(!(size % 4));
	for (int i = 0; i < global_asset_collection.fonts.length; i++)
	{
		font *font_at = array_at(&global_asset_collection.fonts, i);
		
		if (start_addr == font_at->start_addr && font_at->size == size)
		{
			// font is already loaded/loading
			font_at->references++;
			return font_at;
		}
	}
	
	font new_font;
	new_font.loaded = false;
	new_font.start_addr = start_addr;
	new_font.end_addr = end_addr;
	new_font.size = size;
	new_font.references = 1;
	
	// NOTE(Aldrik): we should never realloc the font array because pointers will be 
	// invalidated.
	assert(global_asset_collection.fonts.reserved_length > global_asset_collection.fonts.length);
	
	int index = array_push(&global_asset_collection.fonts, &new_font);
	
	asset_task task;
	task.type = ASSET_FONT;
	task.font = array_at(&global_asset_collection.fonts, index);
	
	mutex_lock(&asset_mutex);
	array_push(&global_asset_collection.queue.queue, &task);
	mutex_unlock(&asset_mutex);
	
	return task.font;
}

void assets_destroy_font(font *font_to_destroy)
{
	if (font_to_destroy->references == 1)
	{
		//glBindTexture(GL_TEXTURE_2D, 0);
		//glDeleteTextures(1, font_to_destroy->textureIDs);
	}
	else
	{
		font_to_destroy->references--;
	}
}

void assets_destroy()
{
	global_asset_collection.valid = false;
	global_asset_collection.done_loading_assets = false;
	
	array_destroy(&global_asset_collection.images);
	array_destroy(&global_asset_collection.fonts);
	
	array_destroy(&global_asset_collection.queue.queue);
	array_destroy(&global_asset_collection.post_process_queue);
	
	mem_free(binary_path);
	
	mutex_destroy(&asset_mutex);
}


image *assets_load_bitmap(u8 *start_addr, s32 width, s32 height, s32 channels)
{
	// check if image is already loaded or loading
	for (int i = 0; i < global_asset_collection.images.length; i++)
	{
		image *img_at = array_at(&global_asset_collection.images, i);
		
		if (start_addr == img_at->start_addr)
		{
			// image is already loaded/loading
			img_at->references++;
			return img_at;
		}
	}
	
	image new_image;
	new_image.loaded = false;
	new_image.start_addr = start_addr;
	new_image.end_addr = 0;
	new_image.references = 1;
	new_image.keep_in_memory = false;
	new_image.channels = channels;
	new_image.width = width;
	new_image.height = height;
	
	// NOTE(Aldrik): we should never realloc the image array because pointers will be 
	// invalidated.
	assert(global_asset_collection.images.reserved_length > global_asset_collection.images.length);
	
	int index = array_push(&global_asset_collection.images, &new_image);
	
	asset_task task;
	task.type = ASSET_BITMAP;
	task.image = array_at(&global_asset_collection.images, index);
	
	mutex_lock(&asset_mutex);
	array_push(&global_asset_collection.queue.queue, &task);
	mutex_unlock(&asset_mutex);
	
	return task.image;
}

void assets_destroy_bitmap(image *image_to_destroy)
{
	if (image_to_destroy->references == 1)
	{
		glBindTexture(GL_TEXTURE_2D, 0);
		glDeleteTextures(1, &image_to_destroy->textureID);
		
		//array_remove(&global_asset_collection.images, image_at);
	}
	else
	{
		image_to_destroy->references--;
	}
}
