#include "actions.h"
#include "ev3.h"
#include "ev3_tacho.h"
#include "sig_shared.h"
#include <unistd.h>

// NOTE: ramping flag is bugged or i dont know how to use it
void flip(uint8_t scanny_sn, uint8_t flippy_sn)
{
    if(!keep_running)
        return;
    FLAGS_T state;
    set_tacho_position_sp(scanny_sn, 0);
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

void negate_polarity(uint8_t sn)
{
    INX_T pol = get_tacho_polarity_inx(sn);
    if (pol == TACHO_NORMAL)
        set_tacho_polarity_inx(sn, TACHO_INVERSED);
    if (pol == TACHO_INVERSED)
        set_tacho_polarity_inx(sn, TACHO_NORMAL);
    // otherwise not supported
}

void rot_90(uint8_t spinny_sn)
{
    if(!keep_running)
        return;
    FLAGS_T state;
    set_tacho_position_sp(spinny_sn, 90);
    set_tacho_command_inx(spinny_sn, TACHO_RUN_TO_REL_POS);
    do
    {
        get_tacho_state_flags(spinny_sn, &state);
        usleep(10000);
    } while (state & (~TACHO_RAMPING));
}

void rot_180(uint8_t spinny_sn)
{
    if(!keep_running)
        return;
    FLAGS_T state;
    set_tacho_position_sp(spinny_sn, 90);
    set_tacho_command_inx(spinny_sn, TACHO_RUN_TO_REL_POS);
    do
    {
        get_tacho_state_flags(spinny_sn, &state);
        usleep(10000);
    } while (state & (~TACHO_RAMPING));
}

void rot_counter_90(uint8_t spinny_sn)
{
    if(!keep_running)
        return;
    FLAGS_T state;
    negate_polarity(spinny_sn);
    set_tacho_position_sp(spinny_sn, 90);
    set_tacho_command_inx(spinny_sn, TACHO_RUN_TO_REL_POS);
    do
    {
        get_tacho_state_flags(spinny_sn, &state);
        usleep(10000);
    } while (state & (~TACHO_RAMPING));
    negate_polarity(spinny_sn);
}
