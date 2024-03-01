#include "actions.h"
#include "ev3.h"
#include "ev3_port.h"
#include "ev3_tacho.h"
#include "scan_routine.h"
#include "serial_read.h"
#include "sig_shared.h"
#include "viz_col.h"
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void set_sane_defaults(uint8_t sn)
{
    int max_speed;
    //set_tacho_command_inx(sn, TACHO_RESET);
    //set_tacho_position(sn, 0);
    //set_tacho_position_sp(sn, 0);
    get_tacho_max_speed(sn, &max_speed);
    set_tacho_speed_sp(sn, max_speed / 5);
    set_tacho_ramp_up_sp(sn, 1000);
    set_tacho_ramp_down_sp(sn, 1000);
    set_tacho_stop_action_inx(sn, TACHO_BRAKE);
}

int main()
{
    char s[256];
    int i;
    uint8_t spinny_sn;
    uint8_t scanny_sn;
    uint8_t flippy_sn;

    printf("Initing robot\n");
    if (ev3_init() == -1)
    {
        exit(1);
    }
    printf("Initiated robot\n");
    signal(SIGINT, sig_handler);

    if ((i = ev3_tacho_init()) != 3)
    {
        printf("error with tachos, %d\n"
               "make sure spin2win is port B,\n"
               "flipper is port C,\n"
               "and scanner is port D.\n",
               i);
        exit(2);
    }
    printf("found ports:\n");
    for (i = 0; i < DESC_LIMIT; i++)
    {
        if (ev3_tacho[i].type_inx != TACHO_TYPE__NONE_)
        {
            printf(" type = %s\n", ev3_tacho_type(ev3_tacho[i].type_inx));
            printf(" port = %s\n", ev3_tacho_port_name(i, s));
        }
    }
    ev3_search_tacho_plugged_in(OUTPUT_B, 0, &spinny_sn, 0);
    ev3_search_tacho_plugged_in(OUTPUT_C, 0, &flippy_sn, 0);
    ev3_search_tacho_plugged_in(OUTPUT_D, 0, &scanny_sn, 0);
    set_sane_defaults(spinny_sn);
    set_sane_defaults(flippy_sn);
    set_sane_defaults(scanny_sn);
    usleep(1000000);

    FLAGS_T state;
    int spinny_pos, flippy_pos, scanny_pos;
    get_tacho_position(flippy_sn, &flippy_pos);
    get_tacho_position(spinny_sn, &spinny_pos);
    get_tacho_position(scanny_sn, &scanny_pos);
    printf("starting at flippy_pos = %d\n"
           "\tspinny_pos = %d\n"
           "\tscanny_pos = %d\n",
           flippy_pos, spinny_pos, scanny_pos);

    pthread_t data_handler;
    pthread_t window_handler;
    pthread_create(&data_handler, NULL, &thread_wrap_com, NULL);
    pthread_create(&window_handler, NULL, &thread_wrap_show_col, NULL);

    //while(keep_running)
    //{
    //    flip(scanny_sn, flippy_sn);
    //}
    scan(scanny_sn, spinny_sn, flippy_sn);

    set_tacho_position_sp(scanny_sn, 0);
    set_tacho_command_inx(scanny_sn, TACHO_RUN_TO_ABS_POS);
    do
    {
        get_tacho_state_flags(scanny_sn, &state);
        usleep(10000);
    } while (state & (~TACHO_RAMPING));

    get_tacho_position(spinny_sn, &spinny_pos);
    spinny_pos%=90;
    spinny_pos*=-1;
    set_tacho_position_sp(spinny_sn, spinny_pos);
    set_tacho_command_inx(spinny_sn, TACHO_RUN_TO_REL_POS);
    do
    {
        get_tacho_state_flags(spinny_sn, &state);
        usleep(10000);
    } while (state & (~TACHO_RAMPING));

    printf("joining dispatched threads before terminating\n");
    pthread_join(data_handler, NULL);
    pthread_join(window_handler, NULL);
    ev3_uninit();
    printf("uninitiated\n");
    return 0;
}
