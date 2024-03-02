#include "ev3.h"
#include "ev3_port.h"
#include "ev3_tacho.h"
#include "scan_routine.h"
#include "serial_read.h"
#include "sig_shared.h"
#include "turner.h"
#include "twister.h"
#include "viz_col.h"
#include <pthread.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_SPEED 1050

void set_sane_defaults(uint8_t sn)
{
    // set_tacho_command_inx(sn, TACHO_RESET);
    //  set_tacho_position(sn, 0);
    //  set_tacho_position_sp(sn, 0);
    set_tacho_speed_sp(sn, MAX_SPEED / 5);
    set_tacho_ramp_up_sp(sn, 1000);
    set_tacho_ramp_down_sp(sn, 1000);
    set_tacho_stop_action_inx(sn, TACHO_BRAKE);
}

int main(int argc, char **argv)
{
    char *movestring;
    char opt;
    bool twist_opt = 0;
    bool scan_opt = 0;
    while ((opt = getopt(argc, argv, "t:s")) != -1)
    {
        switch (opt)
        {
        case 't':
            twist_opt = 1;
            movestring = malloc(strlen(optarg) + 1);
            movestring = strcpy(movestring, optarg);
            break;
        case 's':
            scan_opt = 1;
            break;
        default: {
        usage: // maybe wrap in a function instead of goto, but then you need to pass argv[0]
            fprintf(stderr,
                    "Usage: %s [-t <solution string>] to solve a cube XOR %s [-s] to scan cube.\n"
                    "Spinner needs to be port B, flipper port C and scanner is port D.\n",
                    argv[0], argv[0]);
            exit(1);
        }
        }
    }
    if (!(twist_opt ^ scan_opt))
    {
        if (twist_opt)
            free(movestring);
        goto usage;
    }
    int i;
    uint8_t spinny_sn, scanny_sn, flippy_sn;

    printf("Initing robot\n");
    if (ev3_init() == -1)
    {
        exit(1);
    }
    printf("Initiated robot\n");
    signal(SIGINT, sig_handler);

    if (((i = ev3_tacho_init()) < 3))
    {
        printf("Atleast three motors are needed .\n");
        ev3_uninit();
        exit(1);
    }
    ev3_search_tacho_plugged_in(OUTPUT_B, 0, &spinny_sn, 0);
    ev3_search_tacho_plugged_in(OUTPUT_C, 0, &flippy_sn, 0);
    set_sane_defaults(spinny_sn);
    set_sane_defaults(flippy_sn);
    set_tacho_speed_sp(flippy_sn, MAX_SPEED / 8); // flipper needs to go slower than the other motors
    ev3_search_tacho_plugged_in(OUTPUT_D, 0, &scanny_sn, 0);
    set_sane_defaults(scanny_sn);
    usleep(500000);

    FLAGS_T state;
    int spinny_pos;
#ifdef DEBUG
    int flippy_pos, scanny_pos;
    get_tacho_position(flippy_sn, &flippy_pos);
    get_tacho_position(spinny_sn, &spinny_pos);
    get_tacho_position(scanny_sn, &scanny_pos);
    fprintf(stderr,
            "starting at flippy_pos = %d\n"
            " spinny_pos = %d\n"
            " scanny_pos = %d\n",
            scanny_pos, flippy_pos, spinny_pos);
#endif
    if (scan_opt)
    {
        pthread_t data_handler;
        pthread_t window_handler;
        pthread_create(&data_handler, NULL, &thread_wrap_com, NULL);
        pthread_create(&window_handler, NULL, &thread_wrap_show_col, NULL);
        scan(scanny_sn, spinny_sn, flippy_sn);
        printf("joining dispatched threads before terminating\n");
        pthread_join(data_handler, NULL);
        pthread_join(window_handler, NULL);
        set_tacho_position_sp(scanny_sn, 0);
        set_tacho_command_inx(scanny_sn, TACHO_RUN_TO_ABS_POS);
        do
        {
            get_tacho_state_flags(scanny_sn, &state);
            usleep(10000);
        } while (state & (~TACHO_RAMPING)); // ramping flag is bugged
    }
    else
    {
        struct cube_state init_pos = {.bottom = D, .next = L};
        perform_moves(scanny_sn, flippy_sn, spinny_sn, &init_pos, movestring);
    }
    get_tacho_position(spinny_sn, &spinny_pos);
    spinny_pos %= 90;
    spinny_pos *= -1;
    set_tacho_position_sp(spinny_sn, spinny_pos);
    set_tacho_command_inx(spinny_sn, TACHO_RUN_TO_REL_POS);
    do
    {
        get_tacho_state_flags(spinny_sn, &state);
        usleep(10000);
    } while (state & (~TACHO_RAMPING));

    ev3_uninit();
    printf("uninitiated\n");
    return 0;
}
