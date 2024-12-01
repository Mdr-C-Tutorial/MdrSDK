#include "stdio.h"

int main(int argc, char** argv){
    for (int i = 0; i < argc; i++) {
        printf("Argument: %s\n",argv[i]);
    }
    printf("hello world!\n");
    return 0;
}
