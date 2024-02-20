#include "serial_read.h"
#include "sig_shared.h"
#include <SDL2/SDL.h>
#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

void *thread_wrap_com(void *_)
{
    (void)_;
    (void)communication();
    return NULL;
}

int communication()
{
    const char file_name[] = OUT_FILE;
    const char portpath[] = PORT_NAME;

    // se man 2 open
    int fd = open(portpath, O_RDWR | O_NOCTTY | O_SYNC);
    if (fd < 0)
    {
        fprintf(stderr, "Error opening port %s: %s\n", portpath, strerror(errno));
        exit(1);
    }
    struct termios tty;
    if (tcgetattr(fd, &tty) < 0)
    {
        fprintf(stderr, "Error getting attributes: %s\n", strerror(errno));
        close(fd);
        exit(2);
    }
    // set attributes.
    // matches arduino default, aka 8 data bits, no parity, one stop bit
    cfsetospeed(&tty, B115200);
    cfsetispeed(&tty, B115200);
    tty.c_cflag |= CLOCAL | CREAD;
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;
    tty.c_cflag &= ~PARENB;
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CRTSCTS;

    tty.c_lflag |= ICANON | ISIG;
    tty.c_lflag &= ~(ECHO | ECHOE | ECHONL | IEXTEN);

    tty.c_iflag &= ~IGNCR;
    tty.c_iflag &= ~INPCK;
    tty.c_iflag &= ~(INLCR | ICRNL | IUCLC | IMAXBEL);
    tty.c_iflag &= ~(IXON | IXOFF | IXANY);

    tty.c_oflag &= ~OPOST;

    tty.c_cc[VEOL] = 0;
    tty.c_cc[VEOL2] = 0;
    tty.c_cc[VEOF] = 0x04;

    if (tcsetattr(fd, TCSANOW, &tty) != 0)
    {
        fprintf(stderr, "Error setting attributes: %s\n", strerror(errno));
        exit(3);
    }

    FILE *file = fopen(file_name, "w");
    while (keep_running)
    {
        char buf[200];
        ssize_t rdl = read(fd, buf, sizeof(buf) - 1);
        if (rdl > 0)
        {
            buf[rdl] = '\0';
            fwrite(buf, 1, rdl, file);
            uint32_t col_tmp;
            sscanf(buf, "%x", &col_tmp);
            col_shared = col_tmp;
            continue;
        }
        if (rdl < 0)
        {
            fprintf(stderr, "Error read: %zd: %s\n", rdl, strerror(errno));
            continue;
        }
        printf("Nothing read\n");
    }
    fprintf(stderr, "see \"%s\" for data read.\n", file_name);
    fclose(file);
    close(fd);
    return 0;
}
