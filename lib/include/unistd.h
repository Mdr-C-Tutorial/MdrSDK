#pragma once

ssize_t read(int fd,void * buf ,size_t count);
int close(int fd);
int open(const char * pathname,int flags);