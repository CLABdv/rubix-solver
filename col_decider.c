#include "col_decider.h"
#include <math.h>
#include <stdint.h>
#include <stdio.h>

double vec_len_sq(double *v)
{
    return v[0] * v[0] + v[1] * v[1] + v[2] * v[2];
}

double vec_len(double *v)
{
    return sqrt(vec_len_sq(v));
}

void vec_sub(const double *v1, const double *v2, double *buf)
{
    for (int i = 0; i < 3; i++)
    {
        buf[i] = v1[i] - v2[i];
    }
}
const double col_vals[18][3] = {

    CORNER_WHITE_VAL,  EDGE_WHITE_VAL,  MIDDLE_WHITE_VAL,

    CORNER_YELLOW_VAL, EDGE_YELLOW_VAL, MIDDLE_YELLOW_VAL,

    CORNER_GREEN_VAL,  EDGE_GREEN_VAL,  MIDDLE_GREEN_VAL,

    CORNER_BLUE_VAL,   EDGE_BLUE_VAL,   MIDDLE_BLUE_VAL,

    CORNER_RED_VAL,    EDGE_RED_VAL,    MIDDLE_RED_VAL,

    CORNER_ORANGE_VAL, EDGE_ORANGE_VAL, MIDDLE_ORANGE_VAL};

enum colour_t converter(enum colour_pos_t col_pos)
{
    switch (col_pos)
    {
    case CORNER_WHITE:
    case EDGE_WHITE:
    case MIDDLE_WHITE:
        return WHITE;
    case CORNER_YELLOW:
    case EDGE_YELLOW:
    case MIDDLE_YELLOW:
        return YELLOW;
    case CORNER_GREEN:
    case EDGE_GREEN:
    case MIDDLE_GREEN:
        return GREEN;
    case CORNER_BLUE:
    case EDGE_BLUE:
    case MIDDLE_BLUE:
        return BLUE;
    case CORNER_RED:
    case EDGE_RED:
    case MIDDLE_RED:
        return RED;
    case CORNER_ORANGE:
    case EDGE_ORANGE:
    case MIDDLE_ORANGE:
        return ORANGE;
    }
}

enum colour_t decider(uint_fast32_t col, enum pos_t pos)
{
    double v[3];
    for (int i = 0; i < 3; i++)
    {
        v[i] = (col & (0xff << (8 * i))) >> (8 * i);
    }
    double vlen = vec_len(v);
    for (int i = 0; i < 3; i++)
    {
        v[i] /= vlen;
    }
    printf("normalised col is [%lf, %lf, %lf], and veclen is %lf\n", v[0], v[1], v[2], vlen);
    double current_min_dist = INFINITY;
    enum colour_pos_t current_col = 0;
    for (int i = 0; i < 6; i++)
    {
        double vtemp[3];
        vec_sub(v, col_vals[i * 3 + pos], vtemp);
        double new_dist = vec_len_sq(vtemp);
        if (new_dist < current_min_dist)
        {
            current_col = i * 3 + pos;
            current_min_dist = new_dist;
        }
    }
    enum colour_t c = converter(current_col);
    // fixes some mess-ups which seem to require more info than the direction
    // i think this makes it somewhat more accurate
    if(c == RED && vlen > 215)
        return ORANGE;
    if(c == ORANGE && vlen < 190)
        return RED;
    if(c == WHITE && vlen < 230)
        return BLUE;
    return c;
}
void print_color(enum colour_t col_id)
{
    switch (col_id)
    {
    case WHITE:
        printf("white\n");
        break;
    case YELLOW:
        printf("yellow\n");
        break;
    case GREEN:
        printf("green\n");
        break;
    case BLUE:
        printf("blue\n");
        break;
    case RED:
        printf("red\n");
        break;
    case ORANGE:
        printf("orange\n");
        break;
    }
}
