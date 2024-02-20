#include "sig_shared.h"
#include <signal.h>
#include <stdatomic.h>
volatile sig_atomic_t keep_running = 1;
volatile atomic_uint_fast32_t col_shared = 0;
void sig_handler(int _)
{
    (void)_;
    keep_running = 0;
}
