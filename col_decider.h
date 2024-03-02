#ifndef COL_DECIDER_H
#define COL_DECIDER_H
#include <stdint.h>
#include <stdio.h>

enum colour_t
{
    WHITE,
    BLUE,
    ORANGE,
    YELLOW,
    GREEN,
    RED,
};

enum pos_t
{
    CORNER,
    EDGE,
    MIDDLE
};

enum colour_pos_t
{
    CORNER_WHITE,
    EDGE_WHITE,
    MIDDLE_WHITE,

    CORNER_BLUE,
    EDGE_BLUE,
    MIDDLE_BLUE,

    CORNER_ORANGE,
    EDGE_ORANGE,
    MIDDLE_ORANGE,

    CORNER_YELLOW,
    EDGE_YELLOW,
    MIDDLE_YELLOW,

    CORNER_GREEN,
    EDGE_GREEN,
    MIDDLE_GREEN,

    CORNER_RED,
    EDGE_RED,
    MIDDLE_RED,
};

#define EDGE_POS -139
#define MIDDLE_POS -156
#define CORNER_POS -139

enum colour_t decider(uint_fast32_t, enum pos_t, FILE *log_file);
void fprint_color(FILE *file, enum colour_t);
void print_pos(enum pos_t pos);
// red and orange are corner vals,
// rest doesnt matter because theyre different enough

#define CORNER_WHITE_VAL {0.610711, 0.603691, 0.512435}
#define EDGE_WHITE_VAL  {0.627110, 0.588263, 0.510568}
#define MIDDLE_WHITE_VAL {0.577350, 0.577350, 0.577350}

#define CORNER_YELLOW_VAL {0.689260, 0.675876, 0.260982}
#define EDGE_YELLOW_VAL {0.698238, 0.671383, 0.248412}
#define MIDDLE_YELLOW_VAL {0.659177, 0.659177, 0.361901}

#define CORNER_GREEN_VAL {0.422667, 0.818497, 0.389122}
#define EDGE_GREEN_VAL {0.489162, 0.788095, 0.373666}
#define MIDDLE_GREEN_VAL {0.356555, 0.856795, 0.372520}

#define CORNER_BLUE_VAL {0.339164, 0.590146, 0.732595}
#define EDGE_BLUE_VAL {0.369681, 0.588751, 0.718824}
#define MIDDLE_BLUE_VAL {0.302436, 0.557826, 0.772892}

#define CORNER_RED_VAL {0.956575, 0.206113, 0.206113}
#define EDGE_RED_VAL {0.801148, 0.417500, 0.428784}
#define MIDDLE_RED_VAL {0.960474, 0.194194, 0.199443}

#define CORNER_ORANGE_VAL {0.954101, 0.247360, 0.168833}
#define EDGE_ORANGE_VAL {0.951700, 0.254849, 0.171226}
#define MIDDLE_ORANGE_VAL {0.909572, 0.342427, 0.235419}

#endif
