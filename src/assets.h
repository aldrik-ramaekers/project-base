/* 
*  BSD 2-Clause “Simplified” License
*  Copyright (c) 2019, Aldrik Ramaekers, aldrik.ramaekers@protonmail.com
*  All rights reserved.
*/

#ifndef INCLUDE_ASSETS
#define INCLUDE_ASSETS

//	:/Title	Assets
//	:/Text	The asset system is responsible for loading images, fonts and sound
//			into memory and the active render device. The asset system is multi-threaded.
//			The amount of threads dedicated to loading assets can be specified with ASSET_WORKER_COUNT,
//			defaulting to 2 threads. All assets that will be used in your application need to be queued
//			into the asset system before starting the main loop. The asset system will automatically shut
//			down when all assets have been loaded.


#ifndef ASSET_IMAGE_COUNT
#define ASSET_IMAGE_COUNT 10
#endif

#ifndef ASSET_FONT_COUNT
#define ASSET_FONT_COUNT 1
#endif

#ifndef ASSET_QUEUE_COUNT
#define ASSET_QUEUE_COUNT 20
#endif

#ifndef ASSET_WORKER_COUNT
#define ASSET_WORKER_COUNT 2
#endif

#ifdef MODE_DEVELOPER
s32 __frames_drawn_with_missing_assets = 0;
#endif


typedef struct t_image {
	u8 *start_addr;
	u8 *end_addr;
	bool loaded;
	s32 width;
	s32 height;
	s32 channels;
	void *data;
	s16 references;
	u32 textureID;
	u32 path_hash; // only defined when image is loaded from path, else UNDEFINED_PATH_HASH.
} image;

#define UNDEFINED_PATH_HASH -1
#define UNDEFINED_START_ADDR (void*)(1)
#define TEXT_CHARSET_START 0
#define TEXT_CHARSET_END 2000
#define TOTAL_GLYPHS TEXT_CHARSET_END-TEXT_CHARSET_START

#define CAN_ADD_NEW_IMAGE() (global_asset_collection.images.reserved_length > global_asset_collection.images.length)

typedef struct t_glyph
{
	s32 width;
	s32 height;
	s32 advance;
	s32 lsb;
	s32 xoff;
	s32 yoff;
	void *bitmap;
	u32 textureID;
} glyph;

typedef struct t_font
{
	u8 *start_addr;
	u8 *end_addr;
	bool loaded;
	s16 references;
	s16 size;
	s32 px_h;
	float32 scale;
	stbtt_fontinfo info;
	glyph glyphs[TOTAL_GLYPHS];
} font;

typedef enum t_asset_task_type
{
	ASSET_IMAGE,
	ASSET_BITMAP,
	ASSET_FONT,
} asset_task_type;

typedef struct t_asset_task
{
	s8 type;
	bool valid;
	union {
		image *image;
		font *font;
	};
} asset_task;

typedef struct t_asset_queue {
	array queue;
} asset_queue;

typedef struct t_assets {
	array images;
	array fonts;
	asset_queue queue;
	array post_process_queue;
	bool valid;
	bool done_loading_assets;
} assets;

char*	binary_path;
mutex 	asset_mutex;
assets 	global_asset_collection;

//	:/Info	Create the asset system.
void 	assets_create();

//	:/Info	Destroy the asset system. All assets will be invalidated.
void 	assets_destroy();

//	:/Info	Handle post-processing on the loaded assets. Should be called on the main thread.
//	:/Ret	Returns true when an asset was post-processed.
bool 	assets_do_post_process();

//	:/Info	Load a png image.
//	:/Ret	Pointer to the loading/loaded image.
image*	assets_load_image(u8 *start_addr, u8 *end_addr);

//	:/Info	Invalidate the given image.
void 	assets_destroy_image(image *image);

//	:/Info	Load a bitmap image.
//	:/Ret	Pointer to the loading/loaded image.
image*	assets_load_bitmap(u8 *start_addr, u8 *end_addr);

//	:/Info	Load a bitmap image.
//	:/Ret	Pointer to the loading/loaded image.
image*	assets_load_bitmap_from_file(char* path);

//	:/Info	Invalidate the given image.
void 	assets_destroy_bitmap(image *image);

//	:/Info	Load a ttf font.
//	:/Ret	Pointer to the loading/loaded font.
font*	assets_load_font(u8 *start_addr, u8 *end_addr, s16 size);

//	:/Info	Invalidate the given font.
void 	assets_destroy_font(font *font);

void*	_assets_queue_worker();
void 	_assets_switch_render_method();

#define load_image(_name, _inmem) assets_load_image(_binary____data_imgs_##_name##_start,_binary____data_imgs_##_name##_end)
#define load_font(_name, _size) assets_load_font(_binary____data_fonts_##_name##_start,_binary____data_fonts_##_name##_end, _size)
#define load_bitmap(_name) assets_load_bitmap(_binary____data_imgs_##_name##_start,_binary____data_imgs_##_name##_end)

#endif