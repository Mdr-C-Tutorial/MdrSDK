#pragma once

typedef enum oflags {
    O_RDONLY,
    O_WRONLY,
    O_RDWR,
    O_CREAT = 4
} oflags_t;

ssize_t read(int fd,void * buf ,size_t count);
int close(int fd);
int open(const char * pathname,int flags);