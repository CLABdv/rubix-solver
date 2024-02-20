#include "main.h"
#include "col_decider.h"
#include "ev3.h"
#include "ev3_port.h"
#include "ev3_tacho.h"
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
    get_tacho_max_speed(sn, &max_speed);
    set_tacho_speed_sp(sn, max_speed / 6);
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
    FLAGS_T state;

    printf("Initing robot\n");
    if (ev3_init() == -1)
    {
        exit(1);
    }
    printf("Initiated robot\n");
    signal(SIGINT, sig_handler);

    if (ev3_tacho_init() != 3)
    {
        printf("make sure spin2win is port B,\n"
               "flipper is port C,\n"
               "and scanner is port D.\n");
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

#if 0
    set_tacho_position_sp(spinny_sn, 45);
    set_tacho_command_inx(spinny_sn, TACHO_RUN_TO_REL_POS);
    do
    {
        get_tacho_state_flags(spinny_sn, &state);
        usleep(10000);
    } while (state & (~TACHO_RAMPING));

    while (keep_running)
    {
        set_tacho_position_sp(scanny_sn, CORNER_POS);
        set_tacho_command_inx(scanny_sn, TACHO_RUN_TO_ABS_POS);
        do
        {
            get_tacho_state_flags(scanny_sn, &state);
            usleep(10000);
        } while (state & (~TACHO_RAMPING)); // ramping flag seems to be bugged
    }
#endif
    i = 0;
    while (keep_running)
    {
        set_tacho_position_sp(scanny_sn, EDGE_POS);
        set_tacho_command_inx(scanny_sn, TACHO_RUN_TO_ABS_POS);
        do
        {
            get_tacho_state_flags(scanny_sn, &state);
            usleep(10000);
        } while (state & (~TACHO_RAMPING)); // ramping flag seems to be bugged
        usleep(1200000);
        print_color(decider(col_shared, EDGE));
        usleep(500000);

        set_tacho_position_sp(spinny_sn, 65);
        set_tacho_command_inx(spinny_sn, TACHO_RUN_TO_REL_POS);
        do
        {
            get_tacho_state_flags(spinny_sn, &state);
            usleep(10000);
        } while (state & (~TACHO_RAMPING));

        set_tacho_position_sp(spinny_sn, -20);
        set_tacho_command_inx(spinny_sn, TACHO_RUN_TO_REL_POS);
        do
        {
            get_tacho_state_flags(spinny_sn, &state);
            usleep(10000);
        } while (state & (~TACHO_RAMPING));

        set_tacho_position_sp(scanny_sn, CORNER_POS);
        set_tacho_command_inx(scanny_sn, TACHO_RUN_TO_ABS_POS);
        do
        {
            get_tacho_state_flags(scanny_sn, &state);
            usleep(10000);
        } while (state & (~TACHO_RAMPING));
        usleep(1200000);
        print_color(decider(col_shared, CORNER));
        usleep(500000);

        set_tacho_position_sp(spinny_sn, 65);
        set_tacho_command_inx(spinny_sn, TACHO_RUN_TO_REL_POS);
        do
        {
            get_tacho_state_flags(spinny_sn, &state);
            usleep(10000);
        } while (state & (~TACHO_RAMPING));

        set_tacho_position_sp(spinny_sn, -20);
        set_tacho_command_inx(spinny_sn, TACHO_RUN_TO_REL_POS);
        do
        {
            get_tacho_state_flags(spinny_sn, &state);
            usleep(10000);
        } while (state & (~TACHO_RAMPING));

        i%=4;
        if(i==0)
        {
            set_tacho_position_sp(scanny_sn, MIDDLE_POS);
            set_tacho_command_inx(scanny_sn, TACHO_RUN_TO_ABS_POS);
            do
            {
                get_tacho_state_flags(scanny_sn, &state);
                usleep(10000);
            } while (state & (~TACHO_RAMPING)); 
            usleep(1200000);
            print_color(decider(col_shared, MIDDLE));
            usleep(500000);

            set_tacho_position_sp(scanny_sn, -75);
            set_tacho_command_inx(scanny_sn, TACHO_RUN_TO_ABS_POS);
            do
            {
                get_tacho_state_flags(scanny_sn, &state);
                usleep(10000);
            } while (state & (~TACHO_RAMPING)); 

            set_tacho_position_sp(flippy_sn, FLIP_POSITION);
            set_tacho_command_inx(flippy_sn, TACHO_RUN_TO_ABS_POS);
            do
            {
                get_tacho_state_flags(flippy_sn, &state);
                usleep(10000);
            } while (state & (~TACHO_RAMPING)); 
            set_tacho_position_sp(flippy_sn, 0);
            set_tacho_command_inx(flippy_sn, TACHO_RUN_TO_ABS_POS);
            do
            {
                get_tacho_state_flags(flippy_sn, &state);
                usleep(10000);
            } while (state & (~TACHO_RAMPING)); 
        }
        ++i;
    }

    set_tacho_position_sp(scanny_sn, 0);
    set_tacho_command_inx(scanny_sn, TACHO_RUN_TO_ABS_POS);
    printf("joining dispatched threads before terminating\n");
    pthread_join(data_handler, NULL);
    pthread_join(window_handler, NULL);
    ev3_uninit();
    printf("uninitiated\n");
    return 0;
}
