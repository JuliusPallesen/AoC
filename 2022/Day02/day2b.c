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

int ctow(char c)
{
    switch (c) {
    case 'X':
        return 0;
    case 'Y':
        return 3;
    case 'Z':
        return 6;
    default:
        return 0;
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

int findRes(int you, int res)
{
    for (int i = 1; i <= 3; i++) {
        if (winner(you, i) == res) {
            return i;
        }
    }
}

/// Day Two of the Advent of Code in C
void main(int argc, char* argv[])
{
    char line[150];
    int len = 0;
    int score = 0;
    int choice = 0;
    char res = '\0';
    char you = '\0';
    if (argc > 1) {
        char* file = argv[1];
        FILE* fp = fopen(file, "r");
        while (fgets(line, 150, fp)) {
            sscanf(line, "%c %c/n", &you, &res);
            score += (findRes(ctoval(you), ctow(res)) + ctow(res));
        }
        fclose(fp);
        free(fp);
        printf("score: %d", score);
    } else {
        printf("Please provide a filepath\n");
    }
}