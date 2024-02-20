#ifndef SIG_SHARED_H
#define SIG_SHARED_H
#include <signal.h>
#include <stdatomic.h>
extern volatile sig_atomic_t keep_running;
extern volatile atomic_uint_fast32_t col_shared;
void sig_handler(int);
#endif
