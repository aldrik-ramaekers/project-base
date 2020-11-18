#include <projectbase/project_base.h>

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

int main(int argc, char** argv) {
    print_h1("String utils");
    print_result("poop", 0);
    print_result("poop", 0);

    if (failure) exit(EXIT_FAILURE);
    return 0;
}