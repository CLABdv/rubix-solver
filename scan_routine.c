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
    char *buf;
    int pos_type;
};

FILE *log_file;

void move_scan(struct move_scan_args *msa, int i)
{
    if(!keep_running)
        return;
    uint8_t scanny_sn = msa->scanny_sn;
    uint8_t spinny_sn = msa->spinny_sn;
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
    msa->buf[i] = enum_to_char(decider(col_shared, pos_type, log_file));
    set_tacho_position_sp(spinny_sn, 45);
    set_tacho_command_inx(spinny_sn, TACHO_RUN_TO_REL_POS);
    do
    {
        get_tacho_state_flags(spinny_sn, &state);
        usleep(10000);
    } while (state & (~TACHO_RAMPING));
}

void scan_face(struct move_scan_args msa, int *order, int offset) //order is length 8
{
    struct move_scan_args msa_edge = msa;
    msa_edge.pos_type=EDGE;
    struct move_scan_args msa_corner = msa;
    msa_corner.pos_type=CORNER;
    for (int i=0; i<4; i++)
    {
        move_scan(&msa_edge, order[2 * i] + offset);
        move_scan(&msa_corner, order[2 * i + 1] + offset);
    }
}

void scan(uint8_t scanny_sn, uint8_t spinny_sn, uint8_t flippy_sn)
{
    log_file = fopen("log.data", "a");
    // refer to https://github.com/efrantar/rob-twophase/blob/master/src/face.h for format
    char buf[]="UUUUUUUUURRRRRRRRRFFFFFFFFFDDDDDDDDDLLLLLLLLLBBBBBBBBB"; // solved string
    int order[8]={1,2,5,8,7,6,3,0};
    struct move_scan_args msa = {scanny_sn, spinny_sn, buf}; // pos_type is undeclared, it is set inside scan_face
    // scanning in this order because then i wont need to change the order
    // the facelets are scanned (i wont need to change the "order" variable)
    
    // top
    scan_face(msa, order, 0);

    rot_90(spinny_sn);
    flip(scanny_sn, flippy_sn);
    rot_counter_90(spinny_sn);

    // front
    scan_face(msa, order, 18);

    flip(scanny_sn, flippy_sn);

    //right
    scan_face(msa, order, 9);

    flip(scanny_sn, flippy_sn);

    // behind
    scan_face(msa, order, 45);

    flip(scanny_sn, flippy_sn);

    // left
    scan_face(msa, order, 36);
    
    rot_90(spinny_sn);
    flip(scanny_sn, flippy_sn);
    rot_counter_90(spinny_sn);
    
    //down
    scan_face(msa, order, 27);

    printf("%s\n", buf);
    fclose(log_file);
}
