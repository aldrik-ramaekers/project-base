/* 
*  BSD 2-Clause “Simplified” License
*  Copyright (c) 2019, Aldrik Ramaekers, aldrik.ramaekers@protonmail.com
*  All rights reserved.
*/

#ifndef INCLUDE_ADDON_TEST_HELPER
#define INCLUDE_ADDON_TEST_HELPER

#define error_if(e) if (e) { printf("       --! ERROR AT: %s\n", #e); return 1; };
#define success return 0;

#define CONFIG_DIRECTORY "test_program_config"

bool failure = false;
void print_h1(char *str) {
    printf("# %s\n", str);
}

void print_result(char *str, s32 result) {
    if (!result) {
        printf("    %s\n", str);
    }
    else {
        printf("    %s - FAILURE\n", str);
        failure = true;
    }
}

#endif