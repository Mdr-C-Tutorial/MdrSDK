#pragma once

#define O_RDONLY 0x0000
#define O_WRONLY 0x0001
#define O_RDWR 0x0002
#define O_APPEND 0x0008
#define O_CREAT 0x0100
#define O_TRUNC 0x0200
#define O_EXCL 0x0400
#define O_TEXT 0x4000
#define O_BINARY 0x8000
#define O_WTEXT 0x10000
#define O_U16TEXT 0x20000
#define O_U8TEXT 0x40000
#define O_ACCMODE (_O_RDONLY|_O_WRONLY|_O_RDWR)

#include "ctype.h"

typedef long off_t;

ssize_t read(int fd,void * buf ,size_t count);
int close(int fd);
int open(const char * pathname,int flags);
int write(int fd, const void *msg, int len);
void _exit(int code);