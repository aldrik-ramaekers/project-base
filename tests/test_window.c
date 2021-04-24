static void update_render_main(platform_window* window)
{

}

s32 test_open_window(int argc, char** argv) {
    platform_init(argc, argv, CONFIG_DIRECTORY);

    platform_window *window = platform_open_window("Example1", 
                500, 500, 800, 600, 500, 500, update_render_main);

    if (platform_is_graphical()) {
        error_if(!platform_window_is_valid(window));
    }

    platform_destroy_window(window);
    platform_destroy();

    success;
}