#include "turner.h"
#include "actions.h"
#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#ifdef DEBUG
#endif

#ifdef DEBUG
void print_cube_face(enum faces face)
{
    switch (face)
    {
    case 0:
        printf("U");
        return;
    case 1:
        printf("F");
        return;
    case 2:
        printf("R");
        return;
    case 3:
        printf("D");
        return;
    case 4:
        printf("B");
        return;
    case 5:
        printf("L");
        return;
    }
}

void print_cube_state(struct cube_state state)
{
    printf("next is ");
    print_cube_face(state.next);
    printf(", bottom is ");
    print_cube_face(state.bottom);
    printf("\n");
}
#endif
// clockwise order of different axes
// the order of the face opposite to face A is face A's order reversed.
static const enum faces order_up_down[] = {F, L, B, R};
static const enum faces order_front_down[] = {U, R, D, L};
static const enum faces order_right_down[] = {D, F, U, B};

// for use when rotating
void change_next(int steps, struct cube_state *state)
{
    int reverse_coeff = 1;
    if (state->bottom == D || state->bottom == B || state->bottom == L)
        reverse_coeff = -1;
    enum faces order_used[4];
    switch (state->bottom)
    {
    case U:
    case D:
        for (int i = 0; i < 4; i++)
            order_used[i] = order_up_down[i];
        break;
    case L:
    case R:
        for (int i = 0; i < 4; i++)
            order_used[i] = order_right_down[i];
        break;
    case F:
    case B:
        for (int i = 0; i < 4; i++)
            order_used[i] = order_front_down[i];
        break;
    }
    int current_index = 0;
    for (; order_used[current_index] != state->next; current_index++)
        ;
    int next_index = current_index + steps * reverse_coeff;
    if (next_index < 0)
        next_index += 4;
    state->next = order_used[next_index];
}

// turns cube such that dest is at the bottom
void navigate_to(uint8_t scanny_sn, uint8_t flippy_sn, uint8_t spinny_sn, enum faces dest, struct cube_state *state)
{
    if (dest == state->bottom)
        return;
    if ((dest + 3) % 6 == state->bottom) // the face which is up is dest
    {
        flip(scanny_sn, flippy_sn);
        flip(scanny_sn, flippy_sn);
        state->bottom = dest;
        state->next = (state->next + 3) % 6;
        return;
    }
    // this is the instances where we need to do opposite of what is in the list
    // therefore we just inverse spinny here and inverse it again after turning to the correct position
    if (state->bottom == D || state->bottom == B || state->bottom == L)
        negate_polarity(spinny_sn);
    enum faces order_used[4];
    switch (state->bottom)
    {
    case U:
    case D:
        for (int i = 0; i < 4; i++)
            order_used[i] = order_up_down[i];
        break;
    case L:
    case R:
        for (int i = 0; i < 4; i++)
            order_used[i] = order_right_down[i];
        break;
    case F:
    case B:
        for (int i = 0; i < 4; i++)
            order_used[i] = order_front_down[i];
        break;
    }
    int current_index;
    int dest_index;
    for (int i = 0; i < 4; i++)
    {
        if (order_used[i] == dest)
            dest_index = i;
        if (order_used[i] == state->next)
            current_index = i;
    }
    int turns = (current_index - dest_index) % 4;
    if (turns < 0)
        turns += 4;
    switch (turns)
    {
    case 0:
        break;
    case 1:
        rot_90(spinny_sn);
        break;
    case 2:
        rot_180(spinny_sn);
        break;
    case 3:
        rot_counter_90(spinny_sn);
        break;
    }
    flip(scanny_sn, flippy_sn);
    // reverse back the polarity
    if (state->bottom == D || state->bottom == B || state->bottom == L)
    {
        negate_polarity(spinny_sn);
        usleep(100000); // just to make sure polarity change goes through
    }
    // if dest is a face which is neither currently up nor down,
    // then the new next is going to be the face which was up when func was called
    state->next = (state->bottom + 3) % 6;
    state->bottom = dest;
}
