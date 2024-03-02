#ifndef ACTIONS_H
#define ACTIONS_H
#include <stdint.h>

void flip(uint8_t scanny_sn, uint8_t flippy_sn);
void rot_90(uint8_t spinny_sn);
void rot_180(uint8_t spinny_sn);
void rot_counter_90(uint8_t spinny_sn);

void twist_90(uint8_t scanny_sn, uint8_t flippy_sn, uint8_t spinny_sn);
void twist_180(uint8_t scanny_sn, uint8_t flippy_sn, uint8_t spinny_sn);
void twist_counter_90(uint8_t scanny_sn, uint8_t flippy_sn, uint8_t spinny_sn);
void negate_polarity(uint8_t sn);

#define FLIP_POSITION 133
#define TWIST_POSITION_1 40
#define TWIST_POSITION_2 37

#endif
