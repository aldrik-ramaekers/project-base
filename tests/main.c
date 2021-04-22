#include <projectbase/project_base.h>
#include <projectbase/addons/c_parser.h>
#include <projectbase/addons/test_helper.h>

#define CONFIG_DIRECTORY "test_program_config"

#include "test_string_utils.c"
#include "test_threads.c"
#include "test_window.c"
#include "test_array.c"
#include "test_c_parser.c"
#include "test_settings_config.c"

int main(int argc, char** argv) {

    print_h1("String utils");
    print_result("String to number", test_string_to_number());
    print_result("String contains", test_string_contains());

    print_h1("Threads");
    print_result("Detached thread", test_detached_thread(argc, argv));
    print_result("Joined thread", test_joined_thread(argc, argv));

    print_h1("Platform");
    print_result("Open window", test_open_window(argc, argv));
    
    print_h1("Array");
    print_result("Write", test_array_write());
    print_result("Read", test_array_read());
    print_result("Delete", test_array_delete());
    print_result("Swap", test_array_swap());
    print_result("Multi-threaded Write", test_array_thread_write());

    print_h1("Settings Config");
    print_result("Write", test_settings_config_write(argc, argv));
    print_result("Read", test_settings_config_read(argc, argv));

    if (failure) exit(EXIT_FAILURE);
    return 0;
}