#include "ctype.h"
#include "math.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

#define STB_SPRINTF_IMPLEMENTATION
#include "stb_sprintf.h"

int snprintf(char *buf,int count, const char *fmt, ...){
    va_list args;
    int i;
    va_start(args, fmt);
    i = stbsp_vsnprintf(buf,count, fmt, args);
    va_end(args);
    return i;
}

int sprintf(char *buf, const char *fmt, ...) {
    va_list args;
    int i;
    va_start(args, fmt);
    i = stbsp_vsprintf(buf, fmt, args);
    va_end(args);
    return i;
}

int vsprintf(char *buf, const char *fmt, va_list args) {
    return stbsp_vsprintf(buf,fmt,args);
}

int vsnprintf(char *buf, size_t n, const char *fmt, va_list ap) {
    return stbsp_vsnprintf(buf,n,fmt,ap);
}