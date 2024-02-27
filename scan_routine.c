#include "scan_routine.h"
#include "actions.h"
#include "col_decider.h"
#include "ev3.h"
#include "ev3_tacho.h"
#include "sig_shared.h"
#include <stdio.h>
#include <unistd.h>

// observe that white is assumed to be up, blue is assumed to be front, etc
// TODO: don't assume that
char enum_to_char(enum colour_t col)
{
    switch (col)
    {
    case WHITE:
        return 'U';
    case BLUE:
        return 'F';
    case ORANGE:
        return 'R';
    case YELLOW:
        return 'D';
    case GREEN:
        return 'B';
    case RED:
        return 'L';
    }
}

struct move_scan_args
{
    uint8_t scanny_sn;
    uint8_t spinny_sn;
    char *sing_buf;
    int pos_type;
};

FILE *log_file;

void move_scan(struct move_scan_args *msa, int i)
{
    if(!keep_running)
        return;
    uint8_t scanny_sn = msa->scanny_sn;
    uint8_t spinny_sn = msa->spinny_sn;
    char *sing_buf = msa->sing_buf;
    int pos_type = msa->pos_type;
    int pos_pos;
    FLAGS_T state;
    if (pos_type == CORNER)
        pos_pos = CORNER_POS;
    else
        pos_pos = EDGE_POS;
    set_tacho_position_sp(scanny_sn, pos_pos);
    set_tacho_command_inx(scanny_sn, TACHO_RUN_TO_ABS_POS);
    do
    {
        get_tacho_state_flags(scanny_sn, &state);
        usleep(10000);
    } while (state & (~TACHO_RAMPING)); // ramping flag is bugged or i dont know how to use it
    usleep(1200000);
    sing_buf[i] = enum_to_char(decider(col_shared, pos_type, log_file));
    set_tacho_position_sp(spinny_sn, 45);
    set_tacho_command_inx(spinny_sn, TACHO_RUN_TO_REL_POS);
    do
    {
        get_tacho_state_flags(spinny_sn, &state);
        usleep(10000);
    } while (state & (~TACHO_RAMPING));
}

// horror
// abomination
// disgust
void scan(uint8_t scanny_sn, uint8_t spinny_sn, uint8_t flippy_sn)
{

    log_file = fopen("log.data", "a");
    // singmaster notation, see cube20.org/cubepos.pdf
    char sing_solved[] = "UF UR UB UL DF DR DB DL FR FL BR BL UFR URB UBL ULF DRF DFL DLB DBR";
    char sing_buf[] = "XX XX XX XX XX XX XX XX XX XX XX XX XXX XXX XXX XXX XXX XXX XXX XXX";
    // i miss currying
    struct move_scan_args msa_edge = {scanny_sn, spinny_sn, sing_buf, EDGE};
    struct move_scan_args msa_corner = {scanny_sn, spinny_sn, sing_buf, CORNER};

    // top
    move_scan(&msa_edge, 6);
    move_scan(&msa_corner, 40);
    move_scan(&msa_edge, 3);
    move_scan(&msa_corner, 36);
    move_scan(&msa_edge, 0);
    move_scan(&msa_corner, 48);
    move_scan(&msa_edge, 9);
    move_scan(&msa_corner, 44);

    flip(scanny_sn, flippy_sn);

    // right
    move_scan(&msa_edge, 31);
    move_scan(&msa_corner, 66);
    move_scan(&msa_edge, 16);
    move_scan(&msa_corner, 53);
    move_scan(&msa_edge, 25);
    move_scan(&msa_corner, 38);
    move_scan(&msa_edge, 4);
    move_scan(&msa_corner, 41);

    flip(scanny_sn, flippy_sn);

    // down
    move_scan(&msa_edge, 18);
    move_scan(&msa_corner, 60);
    move_scan(&msa_edge, 21);
    move_scan(&msa_corner, 56);
    move_scan(&msa_edge, 12);
    move_scan(&msa_corner, 52);
    move_scan(&msa_edge, 15);
    move_scan(&msa_corner, 64);

    rot_counter_90(spinny_sn);
    flip(scanny_sn, flippy_sn);

    // behind
    move_scan(&msa_edge, 30);   // BR
    move_scan(&msa_corner, 42); // URB
    move_scan(&msa_edge, 7);    // UB
    move_scan(&msa_corner, 45); // UBL
    move_scan(&msa_edge, 33);   // BL
    move_scan(&msa_corner, 62); // DLB
    move_scan(&msa_edge, 19);   // DB
    move_scan(&msa_corner, 65); // DBR

    rot_90(spinny_sn);
    flip(scanny_sn, flippy_sn);

    // left
    move_scan(&msa_edge, 10);   // UL
    move_scan(&msa_corner, 49); // ULF
    move_scan(&msa_edge, 28);   // FL
    move_scan(&msa_corner, 58); // DFL
    move_scan(&msa_edge, 22);   // DL
    move_scan(&msa_corner, 61); // DLB
    move_scan(&msa_edge, 34);   // BL
    move_scan(&msa_corner, 46); // UBL

    flip(scanny_sn, flippy_sn);

    // front
    move_scan(&msa_edge, 1);   // UF
    move_scan(&msa_corner, 50); // ULF
    move_scan(&msa_edge, 27);   // FL
    move_scan(&msa_corner, 57); // DFL
    move_scan(&msa_edge, 13);   // DF
    move_scan(&msa_corner, 54); // DRF
    move_scan(&msa_edge, 24);   // FR
    move_scan(&msa_corner, 37); // UFR

    printf("sing solved is %s\n"
           "sing scanned is %s\n",
           sing_solved, sing_buf);
    fclose(log_file);
}
