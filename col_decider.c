#include "col_decider.h"
#include <math.h>
#include <stdint.h>
#include <stdio.h>

static double vec_len_sq(double *v)
{
    return v[0] * v[0] + v[1] * v[1] + v[2] * v[2];
}

static double vec_len(double *v)
{
    return sqrt(vec_len_sq(v));
}

static void vec_sub(const double *v1, const double *v2, double *buf)
{
    for (int i = 0; i < 3; i++)
    {
        buf[i] = v1[i] - v2[i];
    }
}

// same order as enum
static const double col_vals[18][3] = {
    CORNER_WHITE_VAL,  EDGE_WHITE_VAL,  MIDDLE_WHITE_VAL,

    CORNER_BLUE_VAL,   EDGE_BLUE_VAL,   MIDDLE_BLUE_VAL,

    CORNER_ORANGE_VAL, EDGE_ORANGE_VAL, MIDDLE_ORANGE_VAL,

    CORNER_YELLOW_VAL, EDGE_YELLOW_VAL, MIDDLE_YELLOW_VAL,

    CORNER_GREEN_VAL,  EDGE_GREEN_VAL,  MIDDLE_GREEN_VAL,

    CORNER_RED_VAL,    EDGE_RED_VAL,    MIDDLE_RED_VAL,

};

static enum colour_t converter(enum colour_pos_t col_pos)
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
    return -1;
}

void print_pos(enum pos_t pos)
{
    switch (pos)
    {
    case 0:
        printf("corner");
        break;
    case 1:
        printf("edge");
        break;
    case 2:
        printf("middle");
        break;
    }
}

enum colour_t decider(uint_fast32_t col, enum pos_t pos, FILE *log_file)
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

    printf("[%lf, %lf, %lf], %lf, ", v[0], v[1], v[2], vlen);
    print_pos(pos);
    printf(", ");
    fprint_color(stdout, c);
    if (log_file != NULL)
    {
        fprintf(log_file, "[%lf, %lf, %lf], %lf, %d, ", v[0], v[1], v[2], vlen, pos);
        fprint_color(log_file, c);
    }
    return c;
}
void fprint_color(FILE *file, enum colour_t col_id)
{
    switch (col_id)
    {
    case WHITE:
        fprintf(file, "white\n");
        break;
    case YELLOW:
        fprintf(file, "yellow\n");
        break;
    case GREEN:
        fprintf(file, "green\n");
        break;
    case BLUE:
        fprintf(file, "blue\n");
        break;
    case RED:
        fprintf(file, "red\n");
        break;
    case ORANGE:
        fprintf(file, "orange\n");
        break;
    }
}
