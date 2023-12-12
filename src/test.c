
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "state.h"
#include "snake_utils.h"

int main (int argc, char** argv) {
    // read board from file
    FILE* fp = fopen("9-everything-in.snk", "r");
    if (fp == NULL) {
        fprintf(stderr, "Failed to open file.\n");
        return 1;
    }
    game_state_t* state = load_board(fp);
    print_board(state, stdout);

    return 0;
}