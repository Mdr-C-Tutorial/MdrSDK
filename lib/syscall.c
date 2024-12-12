#include "syscall.h"

int syscall_open(const char* path,int flags){
    return __syscall(SYSCALL_POSIX_OPEN,path,flags);
}

int syscall_close(int fd){
    return __syscall(SYSCALL_POSIX_CLOSE,fd);
}

ssize_t syscall_read(int fd,void * buf ,size_t count){
    return __syscall(SYSCALL_POSIX_READ,fd,buf,count);
}