
volatile s32 val = 0;

static void* test_t(void *args) {
    thread_sleep(2000);
    val = 50;
}

s32 detached_thread() {
    val = 0;
    thread t = thread_start(test_t, 0);
    thread_detach(&t);
    error_if(val == 50);
    thread_stop(&t);
    success;
}

s32 joined_thread() {
    val = 0;
    thread t = thread_start(test_t, 0);
    thread_join(&t);
    error_if(val != 50);
    success;
}