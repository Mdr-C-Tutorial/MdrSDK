#pragma once

#define SIG_DFL ((void (*)(int))0)

#define SIGINT          2   // interrupt
#define SIGILL          4   // illegal instruction - invalid function image
#define SIGFPE          8   // floating point exception
#define SIGSEGV         11  // segment violation
#define SIGTERM         15  // Software termination signal from kill
#define SIGBREAK        21  // Ctrl-Break sequence
#define SIGABRT         22  // abnormal termination triggered by abort call

typedef int sig_atomic_t;

void (*signal(int sig, void (*func)(int)))(int);
