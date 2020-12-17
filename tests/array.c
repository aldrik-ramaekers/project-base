array arr;
s32 vals[] = {1,2,3,4,5,6,7,8,9};

s32 array_write() {
    
    arr = array_create(sizeof(s32));
    error_if(!array_exists(&arr));

    array_push(&arr, &vals[0]);
    array_push(&arr, &vals[1]);
    array_push(&arr, &vals[2]);
    array_push(&arr, &vals[3]);
    array_push(&arr, &vals[4]);
    array_push(&arr, &vals[8]);

    error_if(arr.length != 6);

    success;
}

s32 array_read() {
    error_if(*(s32*)array_at(&arr, 0) != 1);
    error_if(*(s32*)array_at(&arr, 1) != 2);
    error_if(*(s32*)array_at(&arr, 5) != 9);

    success;
}

s32 array_delete() {
    array_remove_at(&arr, 0);
    error_if(*(s32*)array_at(&arr, 0) != 2);
    error_if(arr.length != 5);

    array_remove_by(&arr, &vals[1]);
    error_if(*(s32*)array_at(&arr, 0) != 3);
    error_if(arr.length != 4);

    success;
}

s32 array_swap_() {
    array_swap(&arr, 0, 3);
    error_if(*(s32*)array_at(&arr, 0) != 9);
    error_if(*(s32*)array_at(&arr, 3) != 3);

    success;
}

void *array_thread_write_imp(void *temp) {
    s32 num = 3;

    for (s32 i = 0; i < 2000; i++) {
        array_push((array*)temp, &num);
    }

    return 0;
}

s32 array_thread_write() {
    array temp = array_create(sizeof(s32));

    thread t1 = thread_start(array_thread_write_imp, &temp);
    thread t2 = thread_start(array_thread_write_imp, &temp);

    thread_join(&t1);
    thread_join(&t2);

    error_if(temp.length != 4000);

    success;
}