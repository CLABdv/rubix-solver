#ifndef ACTIONS_H
#define ACTIONS_H
#include <stdint.h>

void flip(uint8_t scanny_sn, uint8_t flippy_sn);
void rot_90(uint8_t spinny_sn);
void rot_180(uint8_t spinny_sn);
void rot_counter_90(uint8_t spinny_sn);
void negate_polarity(uint8_t sn);
#define EDGE_POS -143
#define MIDDLE_POS -164
#define CORNER_POS -142
#define FLIP_POSITION 130

#endif
