all:
	gcc viz_col.c sig_shared.c serial_read.c col_decider.c main.c -lm -lev3dev-c -lSDL2 -Wall -Wextra -Wpedantic
