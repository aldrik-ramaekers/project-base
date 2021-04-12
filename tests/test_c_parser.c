
s32 test_c_parser_test() {
    char* test = "//cool comments!\n"
    "/* XDDD\n"
    "*/\n"
    "typedef struct t_poop{int meme;int (*func)(int a ,    int b );}poop;void main(int*argc, int **argv) {int p = 0;do{char* = 'f';}while(p++ < argc);for(int i =0; i < 5; i++, i+=2)return 0;}";
    c_token* tokens = c_parser_tokenize(test, strlen(test));
    
    success;
}