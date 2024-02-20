#ifndef SERIAL_READ_H
#define SERIAL_READ_H
#include <signal.h>
#define PORT_NAME "/dev/ttyACM0"
#define OUT_FILE "out.data"
int communication();
void *thread_wrap_com(void *);
#endif
