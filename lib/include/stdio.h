#pragma once

#define EOF -1
#define CANREAD(flag) ((flag)&READ || (flag)&PLUS)
#define CANWRITE(flag) ((flag)&WRITE || (flag)&PLUS || (flag)&APPEND)
#define READ 0x2
#define WRITE 0x4
#define APPEND 0x8
#define BIN 0x0
#define PLUS 0x10
#define ERROR 0x16
#define BUFSIZ (4096*2)

#define SEEK_SET 0    // 文件开头
#define SEEK_CUR 1    // 当前文件指针位置
#define SEEK_END 2    // 文件末尾
#define _IONBF 0x01   // 无缓冲模式
#define _IOFBF 0x02   // 全缓冲模式
#define _IOLBF 0x04   // 行缓冲模式

#define STDOUT_FILENO 256
#define STDERR_FILENO 257
#define STDIN_FILENO 258

#include "ctype.h"

typedef struct FILE {
    int fd;                // 文件描述符
    char *buffer;          // 缓冲区
    size_t size;           // 缓冲区大小
    size_t pos;            // 当前缓冲区位置
    size_t len;            // 缓冲区中有效数据长度
    int flags;             // 文件状态标志
    int ungetc_flag;       // 标记是否有字符被 ungetc
    char ungetc_char;      // 存储被 ungetc 的字符
} FILE;

int printf(const char *format,...);
int putchar(char c);
int puts(char *string);
int vsnprintf(char *buf, size_t n, const char *fmt, va_list ap);
int vsprintf(char *buf, const char *fmt, va_list args);
int sprintf(char *buf, const char *fmt, ...);
int snprintf(char *buf,int count, const char *fmt, ...);

int getc(FILE *file);
int ungetc(int c, FILE *stream);

int setvbuf(FILE *stream, char *buffer, int mode, size_t size);
FILE *tmpfile(void);
int fflush(FILE *stream);
int ferror(FILE *stream);
int fclose(FILE *file);
int fputs(const char *str, FILE *stream);
int fputc(int ch, FILE *stream);
void clearerr(FILE *stream);
FILE *freopen(const char *filename, const char *mode, FILE *stream);
long ftell(FILE *stream);
int fseek(FILE *fp, int offset, int whence);
size_t fread(void *ptr, size_t size, size_t count, FILE *file);
size_t fwrite(const void *ptr, size_t size, size_t count, FILE *stream);
int fprintf(FILE *file, const char *format, ...);
char *fgets(char *str, int n, FILE *file);
int feof(FILE *file);
int fclose(FILE *file);
FILE *fopen(const char *filename, const char *mode);

extern FILE *stdout;
extern FILE *stderr;
extern FILE *stdin;