#ifndef TURNER_H
#define TURNER_H
#include <stdint.h>

// opposite face is (face + 3)%6
enum faces
{
    U,
    F,
    R,
    D,
    B,
    L
};

// we need to keep track of the face which is at the bottom and its rotation
// we do this by also noting which face it can be immidiately turned to
struct cube_state
{
    enum faces bottom;
    enum faces next;
};

void navigate_to(uint8_t scanny_sn, uint8_t flippy_sn, uint8_t spinny_sn, enum faces dest, struct cube_state *state);
void change_next(int steps, struct cube_state *state);
#ifdef DEBUG
void print_cube_state(struct cube_state state);
void print_cube_face(enum faces face);
#endif
#endif
