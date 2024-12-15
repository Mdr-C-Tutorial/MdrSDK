#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"
#include "string.h"
#include "syscall.h"
#include "signal.h"
#include "errno.h"

size_t fread(void *ptr, size_t size, size_t count, FILE *file) {
    if (!file || !CANREAD(file->flags)) return 0;

    size_t total_bytes = size * count;
    size_t bytes_read = 0;
    char *dst = (char *)ptr;

    while (bytes_read < total_bytes) {
        if (file->pos >= file->len) {
            // 填充缓冲区
            if (file->fd == STDIN_FILENO) {
                // 如果是 stdin，使用 syscall_getch
                for (size_t i = 0; i < total_bytes - bytes_read; i++) {
                    dst[bytes_read + i] = syscall_getch();
                }
                bytes_read = total_bytes;
                break;
            } else {
                file->len = read(file->fd, file->buffer, file->size);
                if (file->len == 0) {
                    file->flags |= EOF; // 设置 EOF 标志
                    break;
                }
                if (file->len < 0) {
                    return bytes_read / size; // 返回已成功读取的元素数
                }
                file->pos = 0;
            }
        }

        size_t to_copy = total_bytes - bytes_read;
        size_t available = file->len - file->pos;
        if (to_copy > available) {
            to_copy = available;
        }

        memcpy(dst + bytes_read, file->buffer + file->pos, to_copy);
        file->pos += to_copy;
        bytes_read += to_copy;
    }

    return bytes_read / size;
}

int fprintf(FILE *file, const char *format, ...) {
    if (!file || !CANWRITE(file->flags)) return -1;

    va_list args;
    va_start(args, format);

    char temp_buffer[BUFSIZ];
    int len = vsnprintf(temp_buffer, BUFSIZ, format, args);

    va_end(args);

    if (len > 0) {
        if (file->fd == STDOUT_FILENO || file->fd == STDERR_FILENO) {
            // 如果是 stdout 或 stderr，使用 syscall_print
            syscall_print(temp_buffer);
        } else {
            if (write(file->fd, temp_buffer, len) != len) {
                return -1; // 写入失败
            }
        }
    }

    return len; // 返回写入的字节数
}

size_t fwrite(const void *ptr, size_t size, size_t count, FILE *stream) {
    size_t total_written = 0;    // 记录已写入的字节数
    size_t bytes_to_write = size * count;  // 计算总字节数

    // 如果流不可写，返回0
    if (!CANWRITE(stream->flags)) return 0;

    // 如果是 stdout 或 stderr，跳过缓冲，直接写入
    if (stream->fd == STDOUT_FILENO || stream->fd == STDERR_FILENO) {
        for (int i = 0; i < bytes_to_write; i++) {
            putc(ptr[i],stream);
        }
        return bytes_written;  // 返回写入的字节数
    }

    // 如果缓冲区已经满，需要刷新缓冲区
    while (bytes_to_write > 0) {
        size_t space_left_in_buffer = stream->size - stream->len;

        // 如果当前缓冲区还有剩余空间
        if (space_left_in_buffer > 0) {
            size_t chunk_size = (bytes_to_write < space_left_in_buffer) ? bytes_to_write : space_left_in_buffer;
            // 将数据拷贝到缓冲区
            memcpy(stream->buffer + stream->len, ptr, chunk_size);
            stream->len += chunk_size;
            total_written += chunk_size;
            ptr = (const char *)ptr + chunk_size;
            bytes_to_write -= chunk_size;
        }

        // 如果缓冲区已满，需要写入文件
        if (stream->len == stream->size || bytes_to_write == 0) {
            // 写入缓冲区内容到文件
            ssize_t bytes_written = write(stream->fd, stream->buffer, stream->len);
            if (bytes_written < 0) {
                return total_written;  // 写入失败，返回已写入的字节数
            }
            stream->len = 0;  // 清空缓冲区
        }
    }

    return total_written;  // 返回写入的总字节数
}

int fseek(FILE *fp, int offset, int whence) {
    if (whence == 0) {
        fp->pos = offset;
    } else if (whence == 1) {
        fp->pos += offset;
    } else if (whence == 2) {
        fp->pos = fp->size + offset;
    } else {
        return -1;
    }
    return 0;
}

void clearerr(FILE *stream) {
    if (stream == NULL) {
        return;  // 流无效，直接返回
    }

    // 清除文件流的错误标志
    stream->flags &= ~ERROR;

    // 清除文件流的 EOF 标志
    stream->flags &= ~EOF;
}

int fflush(FILE *stream) {
    if (stream == NULL) {
        return 0;
    }
    if (!(CANWRITE(stream->flags))) {
        return EOF;
    }
    if (stream->fd == STDOUT_FILENO || stream->fd == STDERR_FILENO) {
        return 0;
    }
    if (stream->len > 0) {
        ssize_t bytes_written = write(stream->fd, stream->buffer, stream->len);
        if (bytes_written < 0) {
            return EOF;
        }
        stream->len = 0;
    }
    return 0;
}

int ferror(FILE *stream) {
    if (stream == NULL) {
        return EOF;  // 检查空指针
    }

    // 返回错误标志位
    return (stream->flags & ERROR) != 0;
}

char *fgets(char *str, int n, FILE *file) {
    if (!file || !CANREAD(file->flags)) return NULL;

    char *dst = str;
    int read_count = 0;

    while (read_count < n - 1) {
        if (file->pos >= file->len) {
            // 填充缓冲区
            file->len = read(file->fd, file->buffer, file->size);
            if (file->len == 0) {
                file->flags |= EOF; // 设置 EOF 标志
                break;
            }
            if (file->len < 0) {
                return NULL; // 读取错误
            }
            file->pos = 0;
        }

        if (file->fd == STDIN_FILENO) {
            // 如果是 stdin，使用 syscall_getch
            *dst = syscall_getch();
        } else {
            char c = file->buffer[file->pos++];
            *dst = c;
        }

        if (*dst == '\n') break;
        dst++;
        read_count++;
    }

    if (read_count == 0) return NULL;

    *dst = '\0';
    return str;
}

int feof(FILE *file) {
    return (file->flags & EOF) != 0;
}

FILE *freopen(const char *filename, const char *mode, FILE *stream) {
    // 关闭当前文件流
    if (stream->fd != -1) {
        close(stream->fd);  // 关闭现有的文件描述符
    }

    // 解析打开模式
    int flags = 0;
    if (strcmp(mode, "r") == 0) {
        flags = O_RDONLY;
    } else if (strcmp(mode, "w") == 0) {
        flags = O_WRONLY | O_CREAT | O_TRUNC;
    } else if (strcmp(mode, "a") == 0) {
        flags = O_WRONLY | O_CREAT | O_APPEND;
    } else {
        return NULL;  // 不支持的模式
    }

    // 重新打开文件
    int fd = open(filename, flags);  // 使用相应的权限
    if (fd == -1) {
        return NULL;  // 打开文件失败，返回 NULL
    }

    // 更新文件流的状态
    stream->fd = fd;
    stream->len = 0;          // 清空缓冲区
    stream->flags = 0;        // 清除错误标志

    // 设置缓冲区
    stream->buffer = malloc(BUFSIZ);
    if (stream->buffer == NULL) {
        close(fd);
        return NULL;  // 内存分配失败
    }

    stream->size = BUFSIZ;    // 默认缓冲区大小

    return stream;  // 返回更新后的文件流
}

FILE *tmpfile(void){
    FILE *file = malloc(sizeof(FILE));
    file->flags = O_RDWR;
    file->buffer = (char *)malloc(BUFSIZ);
    if (!file->buffer) {
        free(file);
        return NULL;
    }

    file->size = BUFSIZ;
    file->pos = 0;
    file->len = 0;
    file->flags = 0;
    return file;
}

int setvbuf(FILE *stream, char *buffer, int mode, size_t size) {
    if (stream == NULL) {
        return -1;  // 流无效
    }

    // 如果模式无效，返回错误
    if (mode != _IOFBF && mode != _IOLBF && mode != _IONBF) {
        return -1;
    }

    // 如果当前流已经具有缓冲区，并且重新设置模式
    if (stream->buffer != NULL) {
        free(stream->buffer);  // 释放原有的缓冲区
        stream->buffer = NULL;  // 清空指针
    }

    // 根据模式设置
    switch (mode) {
        case _IONBF:
            // 无缓冲模式，不分配缓冲区
            stream->flags |= _IONBF;
            stream->buffer = NULL;
            stream->size = 0;
            break;

        case _IOFBF:
        case _IOLBF:
            // 分配缓冲区
            if (size == 0) {
                size = BUFSIZ;  // 如果没有指定大小，使用默认大小
            }
            stream->flags |= mode;
            stream->buffer = buffer ? buffer : malloc(size);  // 使用用户提供的缓冲区，或者动态分配一个新的缓冲区
            stream->size = size;
            break;

        default:
            return -1;  // 如果模式不支持
    }

    // 如果分配缓冲区失败
    if (stream->buffer == NULL && mode != _IONBF) {
        return -1;  // 内存分配失败
    }

    // 初始化缓冲区状态
    stream->pos = 0;
    stream->len = 0;

    return 0;
}

int fputc(int ch, FILE *stream) {
    if (stream == NULL) {
        return EOF;
    }

    if (stream == stdout || stream == stderr) {
        if (stream == stderr) {
            if (write(stream->fd, &ch, 1) != 1) {
                return EOF;
            }
            return ch;
        }
        if (stream == stdout && ch == '\n') {
            if (write(stream->fd, &ch, 1) != 1) {
                return EOF;
            }
            if (fflush(stdout) == EOF) {
                return EOF;
            }
            return ch;
        }
    }

    if (stream->flags & _IONBF) {
        if (write(stream->fd, &ch, 1) != 1) {
            return EOF;
        }
        return ch;
    }

    if (stream->flags & _IOLBF && ch == '\n') {
        if (write(stream->fd, &ch, 1) != 1) {
            return EOF;
        }
        if (fflush(stream) == EOF) {
            return EOF;
        }
        return ch;
    }

    if (stream->flags & _IOFBF) {
        if (stream->pos >= stream->size) {
            if (fflush(stream) == EOF) {
                return EOF;
            }
        }
        stream->buffer[stream->pos++] = (char)ch;
    }

    return ch;
}

int fputs(const char *str, FILE *stream) {
    if (str == NULL || stream == NULL) {
        return EOF;
    }
    while (*str) {
        if (fputc(*str, stream) == EOF) {
            return EOF;
        }
        str++;
    }
    return 0;
}

long ftell(FILE *stream) {
    // 检查流是否有效
    if (stream == NULL) {
        errno = EINVAL;  // 无效的流
        return -1L;
    }

    // 如果是未缓冲的流，直接返回文件描述符的当前位置
    if ((stream->flags & _IONBF) != 0) {
        // 使用 lseek 获取文件的当前位置
        long position = fseek(stream, 0, SEEK_CUR);
        if (position == -1) {
            errno = EINVAL;  // 无效操作
            return -1L;
        }
        return position;
    }

    // 对于缓冲流，需要考虑缓冲区的位置
    return stream->pos + (stream->len - stream->pos);  // 当前缓冲区的位置
}

FILE *fopen(const char *filename, const char *mode) {
    int flags = 0;

    if (strcmp(mode, "r") == 0) {
        flags = O_RDONLY;
    } else if (strcmp(mode, "w") == 0) {
        flags = O_WRONLY | O_CREAT | O_TRUNC;
    } else if (strcmp(mode, "a") == 0) {
        flags = O_WRONLY | O_CREAT | O_APPEND;
    } else if (strcmp(mode, "r+") == 0) {
        flags = O_RDWR;
    } else if (strcmp(mode, "w+") == 0) {
        flags = O_RDWR | O_CREAT | O_TRUNC;
    } else if (strcmp(mode, "a+") == 0) {
        flags = O_RDWR | O_CREAT | O_APPEND;
    } else {
        return NULL; // 不支持的模式
    }

    int fd = open(filename, flags);
    if (fd < 0) {
        return NULL;
    }

    FILE *file = (FILE *)malloc(sizeof(FILE));
    if (!file) {
        close(fd);
        return NULL;
    }

    file->fd = fd;
    file->buffer = (char *)malloc(BUFSIZ);
    if (!file->buffer) {
        close(fd);
        free(file);
        return NULL;
    }

    file->size = BUFSIZ;
    file->pos = 0;
    file->len = 0;
    file->flags = 0;

    if (flags & O_RDONLY) file->flags |= READ;
    if (flags & O_WRONLY || flags & O_RDWR) file->flags |= WRITE;

    return file;
}

int fclose(FILE *file) {
    if (!file) return -1;
    close(file->fd);
    free(file->buffer);
    free(file);
    return 0;
}