#include "unistd.h"
#include "syscall.h"

void _exit(int code){
    __syscall(SYSCALL_EXIT, code);
}

int open(const char * pathname,int flags){
    return syscall_open(pathname,flags);
}

int close(int fd){
    return syscall_close(fd);
}

ssize_t read(int fd,void * buf ,size_t count){
    return syscall_read(fd,buf,count);
}

int write(int fd, const void *msg, int len){
    return syscall_write(fd,msg,len);
}

// 扩展函数
int size(int fd){
    return __syscall(SYSCALL_POSIX_SIZEX,fd);
}