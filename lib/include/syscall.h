#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

typedef int              ssize_t;
typedef size_t           usize;
typedef ssize_t          isize;

#define SYSCALL_PUTC 1
#define SYSCALL_PRINT 2
#define SYSCALL_GETCH 3
#define SYSCALL_ALLOC_PAGE 4
#define SYSCALL_FREE 5
#define SYSCALL_EXIT 6
#define SYSCALL_GET_ARG 7
#define SYSCALL_POSIX_OPEN 8
#define SYSCALL_POSIX_CLOSE 9
#define SYSCALL_POSIX_READ 10
#define SYSCALL_POSIX_SIZEX 11
#define SYSCALL_POSIX_WRITE 12
#define SYSCALL_REALLOC 13

#define __syscall0(id)                                                                             \
  ({                                                                                               \
    ssize_t rets;                                                                                  \
    __asm__ volatile("int $31\n\t" : "=a"(rets) : "0"(id) : "memory", "cc");                           \
    rets;                                                                                          \
  })

#define __syscall1(id, arg1)                                                                       \
  ({                                                                                               \
    ssize_t          rets;                                                                         \
    ssize_t          __arg1         = (ssize_t)(arg1);                                             \
    register ssize_t _a1 __asm__("ebx") = __arg1;                                                      \
    __asm__ volatile("int $31\n\t" : "=a"(rets) : "0"(id), "r"(_a1) : "memory", "cc");                 \
    rets;                                                                                          \
  })

#define __syscall2(id, arg1, arg2)                                                                 \
  ({                                                                                               \
    ssize_t          rets;                                                                         \
    ssize_t          __arg1         = (ssize_t)(arg1);                                             \
    ssize_t          __arg2         = (ssize_t)(arg2);                                             \
    register ssize_t _a2 __asm__("ecx") = __arg2;                                                      \
    register ssize_t _a1 __asm__("ebx") = __arg1;                                                      \
    __asm__ volatile("int $31\n\t" : "=a"(rets) : "0"(id), "r"(_a1), "r"(_a2) : "memory", "cc");       \
    rets;                                                                                          \
  })

#define __syscall3(id, arg1, arg2, arg3)                                                           \
  ({                                                                                               \
    ssize_t          rets;                                                                         \
    ssize_t          __arg1         = (ssize_t)(arg1);                                             \
    ssize_t          __arg2         = (ssize_t)(arg2);                                             \
    ssize_t          __arg3         = (ssize_t)(arg3);                                             \
    register ssize_t _a3 __asm__("edx") = __arg3;                                                      \
    register ssize_t _a2 __asm__("ecx") = __arg2;                                                      \
    register ssize_t _a1 __asm__("ebx") = __arg1;                                                      \
    __asm__ volatile("int $31\n\t"                                                                     \
                 : "=a"(rets)                                                                      \
                 : "0"(id), "r"(_a1), "r"(_a2), "r"(_a3)                                           \
                 : "memory", "cc");                                                                \
    rets;                                                                                          \
  })

#define __syscall4(id, arg1, arg2, arg3, arg4)                                                     \
  ({                                                                                               \
    ssize_t          rets;                                                                         \
    ssize_t          __arg1         = (ssize_t)(arg1);                                             \
    ssize_t          __arg2         = (ssize_t)(arg2);                                             \
    ssize_t          __arg3         = (ssize_t)(arg3);                                             \
    ssize_t          __arg4         = (ssize_t)(arg4);                                             \
    register ssize_t _a4 __asm__("esi") = __arg4;                                                      \
    register ssize_t _a3 __asm__("edx") = __arg3;                                                      \
    register ssize_t _a2 __asm__("ecx") = __arg2;                                                      \
    register ssize_t _a1 __asm__("ebx") = __arg1;                                                      \
    __asm__ volatile("int $31\n\t"                                                                     \
                 : "=a"(rets)                                                                      \
                 : "0"(id), "r"(_a1), "r"(_a2), "r"(_a3), "r"(_a4)                                 \
                 : "memory", "cc");                                                                \
    rets;                                                                                          \
  })

#define __syscall5(id, arg1, arg2, arg3, arg4, arg5)                                               \
  ({                                                                                               \
    ssize_t          rets;                                                                         \
    ssize_t          __arg1         = (ssize_t)(arg1);                                             \
    ssize_t          __arg2         = (ssize_t)(arg2);                                             \
    ssize_t          __arg3         = (ssize_t)(arg3);                                             \
    ssize_t          __arg4         = (ssize_t)(arg4);                                             \
    ssize_t          __arg5         = (ssize_t)(arg5);                                             \
    register ssize_t _a5 __asm__("edi") = __arg5;                                                      \
    register ssize_t _a4 __asm__("esi") = __arg4;                                                      \
    register ssize_t _a3 __asm__("edx") = __arg3;                                                      \
    register ssize_t _a2 __asm__("ecx") = __arg2;                                                      \
    register ssize_t _a1 __asm__("ebx") = __arg1;                                                      \
    __asm__ volatile("int $31\n\t"                                                                     \
                 : "=a"(rets)                                                                      \
                 : "0"(id), "r"(_a1), "r"(_a2), "r"(_a3), "r"(_a4), "r"(_a5)                       \
                 : "memory", "cc");                                                                \
    rets;                                                                                          \
  })

#define __syscall_concat_x(a, b)                               a##b
#define __syscall_concat(a, b)                                 __syscall_concat_x(a, b)
#define __syscall_argn_private(_0, _1, _2, _3, _4, _5, n, ...) n
#define __syscall_argn(...)                                    __syscall_argn_private(0, ##__VA_ARGS__, 5, 4, 3, 2, 1, 0)
#define __syscall(id, ...)                                                                         \
  __syscall_concat(__syscall, __syscall_argn(__VA_ARGS__))(id, ##__VA_ARGS__)

int syscall_open(const char* path,int flags);
int syscall_close(int fd);
ssize_t syscall_read(int fd,void * buf ,size_t count);
int syscall_write(int fd, const void *msg, int len);
int syscall_getch();
int syscall_print(char* msg);