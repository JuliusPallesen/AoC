#include <stdio.h>
#include <stdlib.h>

#define ROCK 1
#define PAPER 2
#define SCISSORS 3
#define INVALID 0

int ctoval(char c)
{
    switch (c) {
    case 'A':
    case 'X':
        return ROCK;
        break;
    case 'B':
    case 'Y':
        return PAPER;
        break;
    case 'C':
    case 'Z':
        return SCISSORS;
        break;
    default:
        return INVALID;
        break;
    }
}

int winner(int you, int me)
{
    if (you == me) {
        return 3; // draw
    } else if ((you == ROCK && me == PAPER) || (you == PAPER && me == SCISSORS) || (you == SCISSORS && me == ROCK)) {
        return 6; // me wins
    } else {
        return 0; // you wins
    }
}

/// Day Two of the Advent of Code in C
void main(int argc, char* argv[])
{
    char line[150];
    int len = 0;
    int score = 0;
    char me = '\0';
    char you = '\0';
    if (argc > 1) {
        char* file = argv[1];
        FILE* fp = fopen(file, "r");
        while (fgets(line, 150, fp)) {
            sscanf(line, "%c %c/n", &you, &me);
            score += (ctoval(me) + winner(ctoval(you), ctoval(me)));
        }
        fclose(fp);
        free(fp);
        printf("score: %d", score);
    } else {
        printf("Please provide a filepath\n");
    }
}