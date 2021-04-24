extern unsigned char _binary_examples_logo_64_bmp_start[];
extern unsigned char _binary_examples_logo_64_bmp_end[];

extern unsigned char _binary_examples_logo_64_png_start[];
extern unsigned char _binary_examples_logo_64_png_end[];

extern unsigned char _binary_examples_mono_ttf_start[];
extern unsigned char _binary_examples_mono_ttf_end[];

s32 test_assets_queue_and_postprocess_count(int argc, char** argv) {
	platform_init(argc, argv, CONFIG_DIRECTORY);

	image* img = assets_load_bitmap(_binary_examples_logo_64_bmp_start, _binary_examples_logo_64_bmp_end);
	image* img2 = assets_load_image(_binary_examples_logo_64_png_start, _binary_examples_logo_64_png_end);

	thread_sleep(1000 * 50); // 50ms
	
	error_if(global_asset_collection.images.length == 0);
	error_if(global_asset_collection.post_process_queue.length != 2);
	assets_do_post_process();
	error_if(global_asset_collection.post_process_queue.length != 0);

    platform_destroy();

    success;
}

s32 test_assets_load_embedded(int argc, char** argv) {
	platform_init(argc, argv, CONFIG_DIRECTORY);

	image* img = assets_load_bitmap(_binary_examples_logo_64_bmp_start, _binary_examples_logo_64_bmp_end);
	image* img2 = assets_load_image(_binary_examples_logo_64_png_start, _binary_examples_logo_64_png_end);
	font* fnt = assets_load_font(_binary_examples_mono_ttf_start, _binary_examples_mono_ttf_end, 36);
	error_if(!img);
	error_if(!img2);
	error_if(!fnt);

	thread_sleep(1000 * 50); // 50ms
	assets_do_post_process();

	error_if(!img->loaded);
	error_if(!img2->loaded);
	error_if(!fnt->loaded);

    platform_destroy();

    success;
}

s32 test_assets_load_file(int argc, char** argv) {
	platform_init(argc, argv, CONFIG_DIRECTORY);

	image* img = assets_load_bitmap_from_file("logo_64.bmp");
	image* img2 = assets_load_image_from_file("logo_64.png");
	font* fnt = assets_load_font_from_file("mono.ttf", 48);
	error_if(!img);
	error_if(!img2);
	error_if(!fnt);

	thread_sleep(1000 * 50); // 50ms
	assets_do_post_process();

	error_if(!img->loaded);
	error_if(!img2->loaded);
	error_if(!fnt->loaded);

    platform_destroy();

    success;
}

