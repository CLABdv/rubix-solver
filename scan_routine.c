#include "scan_routine.h"
#include "actions.h"
#include "col_decider.h"
#include "ev3.h"
#include "ev3_tacho.h"
#include "sig_shared.h"
#include <stdio.h>
#include <unistd.h>

static char enum_to_char(enum colour_t col)
{
    switch (col)
    {
    case WHITE:
        return 'W';
    case BLUE:
        return 'B';
    case ORANGE:
        return 'O';
    case YELLOW:
        return 'Y';
    case GREEN:
        return 'G';
    case RED:
        return 'R';
    }
    return -1;
}

struct move_scan_args
{
    uint8_t scanny_sn;
    uint8_t spinny_sn;
    char *buf;
    int pos_type;
};
static FILE *log_file;

static char scan_middle(uint8_t scanny_sn)
{
    if (!keep_running)
        return -1;
    FLAGS_T state;
    set_tacho_position_sp(scanny_sn, MIDDLE_POS);
    set_tacho_command_inx(scanny_sn, TACHO_RUN_TO_ABS_POS);
    do
    {
        get_tacho_state_flags(scanny_sn, &state);
        usleep(10000);
    } while (state & (~TACHO_RAMPING)); // ramping flag is bugged
    usleep(1200000);
    return enum_to_char(decider(col_shared, MIDDLE, log_file));
}

static void move_scan(struct move_scan_args *msa, int i)
{
    if (!keep_running)
        return;
    uint8_t scanny_sn = msa->scanny_sn;
    uint8_t spinny_sn = msa->spinny_sn;
    int pos_type = msa->pos_type;
    int pos_pos;
    FLAGS_T state;
    // this jank is because the values might be equal to each other,
    // therefore we need a name and a value, the name decides the value
    if (pos_type == CORNER)
        pos_pos = CORNER_POS;
    else if (pos_type == EDGE)
        pos_pos = EDGE_POS;
    set_tacho_position_sp(scanny_sn, pos_pos);
    set_tacho_command_inx(scanny_sn, TACHO_RUN_TO_ABS_POS);
    do
    {
        get_tacho_state_flags(scanny_sn, &state);
        usleep(10000);
    } while (state & (~TACHO_RAMPING)); // ramping flag is bugged
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

// note that this function does not scan the middle piece
static void scan_face(struct move_scan_args msa, const int *order, int offset) // order is length 8
{
    struct move_scan_args msa_edge = msa;
    msa_edge.pos_type = EDGE;
    struct move_scan_args msa_corner = msa;
    msa_corner.pos_type = CORNER;
    for (int i = 0; i < 4; i++)
    {
        move_scan(&msa_edge, order[2 * i] + offset);
        move_scan(&msa_corner, order[2 * i + 1] + offset);
    }
}

void scan(uint8_t scanny_sn, uint8_t spinny_sn, uint8_t flippy_sn)
{
    log_file = fopen("log.data", "a");
    // refer to https://github.com/efrantar/rob-twophase/blob/master/src/face.h for format
    char buf[] = "UUUUUUUUURRRRRRRRRFFFFFFFFFDDDDDDDDDLLLLLLLLLBBBBBBBBB"; // solved string
    static const int order[8] = {1, 2, 5, 8, 7, 6, 3, 0};
    char pos_col[6][2] = {{'U', '\0'}, {'F', '\0'}, {'R', '\0'}, {'B', '\0'}, {'L', '\0'}, {'D', '\0'}};
    struct move_scan_args msa = {scanny_sn, spinny_sn, buf}; // pos_type is undeclared, it is set inside scan_face
    // scanning in this order because then i wont need to change the order
    // the facelets are scanned (i wont need to change the "order" variable)

    // up
    pos_col[0][1] = scan_middle(scanny_sn);
    scan_face(msa, order, 0);

    rot_90(spinny_sn);
    flip(scanny_sn, flippy_sn);
    rot_counter_90(spinny_sn);

    // front
    pos_col[1][1] = scan_middle(scanny_sn);
    scan_face(msa, order, 18);

    flip(scanny_sn, flippy_sn);

    // could skip the middle scanning on rest of faces
    // right
    pos_col[2][1] = scan_middle(scanny_sn);
    scan_face(msa, order, 9);

    flip(scanny_sn, flippy_sn);

    // behind
    pos_col[3][1] = scan_middle(scanny_sn);
    scan_face(msa, order, 45);

    flip(scanny_sn, flippy_sn);

    // left
    pos_col[4][1] = scan_middle(scanny_sn);
    scan_face(msa, order, 36);

    rot_90(spinny_sn);
    flip(scanny_sn, flippy_sn);
    rot_counter_90(spinny_sn);

    // down
    pos_col[5][1] = scan_middle(scanny_sn);
    scan_face(msa, order, 27);
    int i;
    // make sure pos_to_col is filled. If not, skip the loop where colours are converted to positions.
    for (i = 0; i < 6; i++)
    {
        if (pos_col[i][1] == '\0')
        {
            printf("Scan went wrong somewhere, colours not correctly decided. Leaving buf as-is.\n");
            goto end;
        }
    }
    i = 0;
    while (buf[i] != '\0')
    {
        for (int j = 0; j < 6; j++)
        {
            if (buf[i] == pos_col[i][1])
            {
                buf[i] = pos_col[i][0];
                break;
            }
        }
        ++i;
    }
end:
    printf("%s\n", buf);
    fclose(log_file);
}
