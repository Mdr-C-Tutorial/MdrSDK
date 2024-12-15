#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "syscall.h"
#include "unistd.h"
#include "setjmp.h"
#include "errno.h"
#include "time.h"
#include "locale.h"

int errno;
FILE *stdout;
FILE *stderr;
FILE *stdin;

char* errno_info[124] = {
    "Operation not permitted",
    "No such file or directory",
    "No such process",
    "Interrupted system call",
    "I/O error",
    "No such device or address",
    "Arg list too long",
    "Exec format error",
    "Bad file number",
    "No child processes",
    "Try again",
    "Out of memory",
    "Permission denied",
    "Bad address",
    "Block device required",
    "Device or resource busy",
    "File exists",
    "Cross-device link",
    "No such device",
    "Not a directory",
    "Is a directory",
    "Invalid argument",
    "File table overflow",
    "Too many open files",
    "Not a typewriter",
    "Text file busy",
    "File too large",
    "No space left on device",
    "Illegal seek",
    "Read-only file system",
    "Too many links",
    "Broken pipe",
    "Math argument out of domain of func",
    "Math result not representable",
    "Resource deadlock would occur",
    "File name too long",
    "No record locks available",
    "Function not implemented",
    "Directory not empty",
    "Too many symbolic links encountered",
    "Operation would block",
    "No message of desired type",
    "Identifier removed",
    "Channel number out of range",
    "Level 2 not synchronized",
    "Level 3 halted",
    "Level 3 reset",
    "Link number out of range",
    "Protocol driver not attached",
    "No CSI structure available",
    "Level 2 halted",
    "Invalid exchange",
    "Invalid request descriptor",
    "Exchange full",
    "No anode",
    "Invalid request code",
    "Invalid slot",
    "Bad font file format",
    "Device not a stream",
    "No data available",
    "Timer expired",
    "Out of streams resources",
    "Machine is not on the network",
    "Package not installed",
    "Object is remote",
    "Link has been severed",
    "Advertise error",
    "Srmount error",
    "Communication error on send",
    "Protocol error",
    "Multihop attempted",
    "RFS specific error",
    "Not a data message",
    "Value too large for defined data type",
    "Name not unique on network",
    "File descriptor in bad state",
    "Remote address changed",
    "Can not access a needed shared library",
    "Accessing a corrupted shared library",
    ".lib section in a.out corrupted",
    "Attempting to link in too many shared libraries",
    "Cannot exec a shared library directly",
    "Illegal byte sequence",
    "Interrupted system call should be restarted",
    "Streams pipe error",
    "Too many users",
    "Socket operation on non-socket",
    "Destination address required",
    "Message too long",
    "Protocol wrong type for socket",
    "Protocol not available",
    "Protocol not supported",
    "Socket type not supported",
    "Operation not supported on transport endpoint",
    "Protocol family not supported",
    "Address family not supported by protocol",
    "Address already in use",
    "Cannot assign requested address",
    "Network is down",
    "Network is unreachable",
    "Network dropped connection because of reset",
    "Software caused connection abort",
    "Connection reset by peer",
    "No buffer space available",
    "Transport endpoint is already connected",
    "Transport endpoint is not connected",
    "Cannot send after transport endpoint shutdown",
    "Too many references: cannot splice",
    "Connection timed out",
    "Connection refused",
    "Host is down",
    "No route to host",
    "Operation already in progress",
    "Operation now in progress",
    "Stale NFS file handle",
    "Structure needs cleaning",
    "Not a XENIX named type file",
    "No XENIX semaphores available",
    "Is a named type file",
    "Remote I/O error",
    "Quota exceeded ",
    "No medium found",
    "Wrong medium type",
};

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

void* realloc(void *cp, size_t nbytes){
    __syscall(SYSCALL_REALLOC,cp,nbytes);
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

int ungetc(int c, FILE *stream) {
    // 检查是否已经有一个字符在 ungetc 中
    if (stream->ungetc_flag) {
        errno = EINVAL;  // 不允许连续调用 ungetc
        return EOF;
    }

    // 将字符放入缓冲区的开头
    stream->ungetc_char = (char)c;
    stream->ungetc_flag = 1;

    return c;  // 返回字符本身
}

int getc(FILE *file) {
    if (!file || !CANREAD(file->flags)) return EOF;  // 检查流是否有效

    if (file->fd == STDIN_FILENO) {
        // 如果是 stdin，使用 syscall_getch
        return syscall_getch();
    } else {
        // 其他流使用 read 系统调用
        if (file->pos >= file->len) {
            file->len = read(file->fd, file->buffer, file->size);
            if (file->len <= 0) {
                return EOF;  // 读取失败或者到达文件末尾
            }
            file->pos = 0;
        }

        // 从缓冲区中返回一个字符
        return (unsigned char)file->buffer[file->pos++];
    }
}

int atoi(const char **s) {
    int i = 0;

    while (isdigit(**s))
        i = i * 10 + *((*s)++) - '0';
    return i;
}

int setjmp(jmp_buf env) {
    __asm__ volatile(
            "movl %%ebp, %0\n"  // 保存当前栈帧指针
            "movl %%esp, %1\n"  // 保存当前栈指针
            "lea 1f, %%eax\n"   // 获取返回地址（即程序计数器）
            "movl %%eax, %2\n"
            "jmp 2f\n"          // 跳到 setjmp 返回
            "1:\n"
            "2:\n"
            : "=m"(env.ebp), "=m"(env.esp), "=m"(env.pc)
            :
            : "eax"
            );

    return 0;  // 初始调用 setjmp 返回时返回 0
}

void longjmp(jmp_buf env, int val) {
    __asm__ volatile(
            "movl %0, %%ebp\n"  // 恢复栈帧指针
            "movl %1, %%esp\n"  // 恢复栈指针
            "movl %2, %%eax\n"  // 恢复返回地址
            "jmp *%%eax\n"      // 跳转到保存的程序计数器位置
            :
            : "m"(env.ebp), "m"(env.esp), "m"(env.pc), "m"(val)
            : "eax"
            );
}

time_t time(time_t *tloc) {
    return (time_t)(-1);
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

char *getenv(const char *name) {
    //TODO 环境变量系统调用
    return NULL;  // 找不到环境变量
}
struct lconv lconv0 = {
        .decimal_point = ".",
        .thousands_sep = ",",
        .grouping = "",
        .int_curr_symbol = "CNY",
        .currency_symbol = "￥",
        .mon_decimal_point = ".",
        .mon_thousands_sep = ",",
        .mon_grouping = "",
        .positive_sign = "",
        .negative_sign = "-",
        .int_frac_digits = 2,
        .frac_digits = 2,
        .p_cs_precedes = 1,
        .p_sep_by_space = 0,
        .n_cs_precedes = 1,
        .n_sep_by_space = 0,
        .p_sign_posn = 1,
        .n_sign_posn = 4,
};


struct lconv *localeconv(void){
    return &lconv0;
}

char *strerror(int errno0){
    if(errno0 < 1 || errno0 > 124) return "Unknown";
    return errno_info[errno0 - 1];
}

void _start() {
    char *cmdline = (char *)__syscall0(SYSCALL_GET_ARG);
    int argc = 0;
    char *argv[50];
    argc = cmd_parse(cmdline, argv, ' ');
    stdout = (FILE *)malloc(sizeof(FILE));
    stdout->fd = STDOUT_FILENO;
    stdout->buffer = (char *)malloc(BUFSIZ);
    stdout->size = BUFSIZ;
    stdout->pos = 0;
    stdout->len = 0;
    stdout->flags = WRITE;  // 设置为可写模式
    stdout->ungetc_flag = 0;

    stderr = (FILE *)malloc(sizeof(FILE));
    stderr->fd = STDERR_FILENO;
    stderr->buffer = (char *)malloc(BUFSIZ);
    stderr->size = BUFSIZ;
    stderr->pos = 0;
    stderr->len = 0;
    stderr->flags = WRITE;  // 设置为可写模式
    stderr->ungetc_flag = 0;

    // 初始化 stdin
    stdin = (FILE *)malloc(sizeof(FILE));
    stdin->fd = STDIN_FILENO;
    stdin->buffer = (char *)malloc(BUFSIZ);
    stdin->size = BUFSIZ;
    stdin->pos = 0;
    stdin->len = 0;
    stdin->flags = READ;
    stdin->ungetc_flag = 0;

    int code = main(argc, argv);
    __syscall(SYSCALL_EXIT, code);
    while (1);
}