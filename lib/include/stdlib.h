#pragma once

#define EXIT_SUCCESS 0    // 程序成功退出
#define EXIT_FAILURE 1    // 程序失败退出

#include "ctype.h"

void exit(int retval);
void *calloc(size_t nelem, size_t elsize);
void *malloc(unsigned size);
void *realloc(void *ptr, unsigned newsize);
void free(void *ptr);
void *xmalloc(size_t size);
int atoi(const char **s);
static inline void abort(void) { exit(-1); }
char* getenv(const char* path);