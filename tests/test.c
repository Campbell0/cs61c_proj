
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


int main (int argc, char** argv) {
    // read board from file
    FILE* fp = fopen("13-sus-in.snk", "r");
    if (fp == NULL) {
        fprintf(stderr, "Failed to open file.\n");
        return 1;
    }

    char* board = malloc(sizeof(char) * 20);
    while(1) {
        board = fgets(board, sizeof(char) * 20, fp);
        if(feof(fp)) {
            break;  
        }

        printf("%s", board);
        printf("board length: %ld\n", strlen(board));
    }
    free(board);
    fclose(fp);
    return 0;
}