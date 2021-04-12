
volatile s32 val = 0;

static void* test_t(void *args) {
    thread_sleep(2000);
    val = 50;
}

s32 test_detached_thread(int argc, char** argv) {
    platform_init(argc, argv);

    val = 0;
    thread t = thread_start(test_t, 0);
    thread_detach(&t);
    error_if(val == 50);
    thread_stop(&t);

    platform_destroy();
    success;
}

s32 test_joined_thread(int argc, char** argv) {
    platform_init(argc, argv);

    val = 0;
    thread t = thread_start(test_t, 0);
    thread_join(&t);
    error_if(val != 50);

    platform_destroy();
    success;
}