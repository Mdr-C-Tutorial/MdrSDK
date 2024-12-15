#ifndef _SET_JMP_H
#define _SET_JMP_H

typedef struct {
    void *ebp;    // 保存栈帧指针
    void *esp;    // 保存栈指针
    void *pc;     // 保存程序计数器（即返回地址）
} jmp_buf;

int setjmp(jmp_buf env);
void longjmp(jmp_buf env, int val);
#endif