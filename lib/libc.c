#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "syscall.h"

void *malloc(size_t size) {
    return (void*)__syscall(SYSCALL_ALLOC_PAGE,size); // TODO malloc implement
}

void *xmalloc(size_t size) {
    void *ptr = malloc(size);
    if (ptr == NULL) exit(1);
    return ptr;
}

void free(void *ptr) {
    // TODO free implement
    __syscall(SYSCALL_FREE,ptr);
}

void *calloc(size_t n, size_t size) {
    void *ptr = malloc(n * size);
    if (ptr == NULL)
        return NULL;
    memset(ptr, 0, n * size);
    return ptr;
}

void exit(int retval) {
    __syscall(SYSCALL_EXIT, retval);
}

int putchar(char c) {
    return __syscall(SYSCALL_PUTC, c);
}

int puts(char *string) {
    return __syscall(SYSCALL_PRINT, string);
}

int printf(const char *fmt, ...) {
    static char buf[4096];
    va_list va;
    va_start(va, fmt);
    int rets = vsprintf(buf, fmt, va);
    va_end(va);
    puts(buf);
    return rets;
}

int atoi(const char **s) {
    int i = 0;

    while (isdigit(**s))
        i = i * 10 + *((*s)++) - '0';
    return i;
}


extern int main(int argc,char** argv);

static inline int cmd_parse(char *cmd_str, char **argv, char token) {
    int arg_idx = 0;
    while (arg_idx < 50) {
        argv[arg_idx] = NULL;
        arg_idx++;
    }
    char *next = cmd_str;
    int argc = 0;

    while (*next) {
        while (*next == token) *next++;
        if (*next == 0) break;
        argv[argc] = next;
        while (*next && *next != token) *next++;
        if (*next) *next++ = 0;
        if (argc > 50) return -1;
        argc++;
    }

    return argc;
}

void _start() {
    char *cmdline = (char *)__syscall0(SYSCALL_GET_ARG);
    int argc = 0;
    char *argv[50];
    argc = cmd_parse(cmdline, argv, ' ');
    int code = main(argc, argv);
    __syscall(SYSCALL_EXIT, code);
    while (1);
}