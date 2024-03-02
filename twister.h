#ifndef TWISTER_H
#define TWISTER_H
#include "turner.h"
#include <stdint.h>
void perform_moves(uint8_t scanny_sn, uint8_t flippy_sn, uint8_t spinny_sn, struct cube_state *state, char *movestring);
#endif
