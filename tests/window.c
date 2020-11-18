
s32 open_window(int argc, char** argv) {
    platform_init(argc, argv);
    settings_init(CONFIG_DIRECTORY);

    platform_window *window = platform_open_window("Example1", 
                500, 500, 800, 600, 500, 500);

    error_if(window == 0);

    platform_destroy_window(window);

    settings_destroy();
    platform_destroy();

    success;
}