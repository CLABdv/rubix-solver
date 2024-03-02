#include "twister.h"
#include "actions.h"
#include "sig_shared.h"
#include "turner.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

// note that movestring is of the format
// R F2 L B2 R' U ...
void perform_moves(uint8_t scanny_sn, uint8_t flippy_sn, uint8_t spinny_sn, struct cube_state *state, char *movestring)
{
    char *tok;
    tok = strtok(movestring, " ");
    while (tok && keep_running)
    {
        switch (tok[0])
        {
        case 'U':
            navigate_to(scanny_sn, flippy_sn, spinny_sn, U, state);
            break;
        case 'F':
            navigate_to(scanny_sn, flippy_sn, spinny_sn, F, state);
            break;
        case 'L':
            navigate_to(scanny_sn, flippy_sn, spinny_sn, L, state);
            break;
        case 'D':
            navigate_to(scanny_sn, flippy_sn, spinny_sn, D, state);
            break;
        case 'B':
            navigate_to(scanny_sn, flippy_sn, spinny_sn, B, state);
            break;
        case 'R':
            navigate_to(scanny_sn, flippy_sn, spinny_sn, R, state);
            break;
        default:
            fprintf(stderr, "error with movestring format\n");
            return;
        }
        switch (tok[1])
        {
        case '\0': // there is no number or prim, therefore it is just a one quarter turn
            twist_90(scanny_sn, flippy_sn, spinny_sn);
            break;
        case '2':
            twist_180(scanny_sn, flippy_sn, spinny_sn);
            break;
        case '\'':
            twist_counter_90(scanny_sn, flippy_sn, spinny_sn);
            break;
        default:
            fprintf(stderr, "error with movestring format\n");
            return;
        }
        tok = strtok(NULL, " ");
    }
}
