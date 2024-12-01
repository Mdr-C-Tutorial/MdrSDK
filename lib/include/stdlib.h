#pragma once

#include "ctype.h"

void exit(int retval);
void *calloc(size_t nelem, size_t elsize);
void *malloc(unsigned size);
void *realloc(void *ptr, unsigned newsize);
void free(void *ptr);
void *xmalloc(size_t size);
int atoi(const char **s);
static inline void abort(void) { exit(-1); }