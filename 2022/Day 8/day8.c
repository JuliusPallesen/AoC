#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define GRIDSIZE 99

typedef int (*grid)[GRIDSIZE][GRIDSIZE];
typedef bool (*bgrid)[GRIDSIZE][GRIDSIZE];

typedef enum dir { TOP,
    BOTTOM,
    LEFT,
    RIGHT } dir;

int getDist(grid trees, int x, int y, dir d)
{
    int i = 0;
    int h = (*trees)[y][x];
    switch (d) {
    case TOP:
        while (1) {
            i++;
            if ((y - i) < 0) {
                i--;
                break;
            } else if ((*trees)[y - i][x] >= h)
                break;
        }
        break;
    case BOTTOM:
        while (1) {
            i++;
            if ((y + i) >= GRIDSIZE) {
                i--;
                break;
            } else if ((*trees)[y + i][x] >= h)
                break;
        }
        break;
    case LEFT:
        while (1) {
            i++;
            if ((x - i) < 0) {
                i--;
                break;
            } else if ((*trees)[y][x - i] >= h)
                break;
        }
        break;
    case RIGHT:
        while (1) {
            i++;
            if ((x + i) >= GRIDSIZE) {
                i--;
                break;
            } else if ((*trees)[y][x + i] >= h)
                break;
        }
        break;
    default:
        printf("INVALID DIR\n");
    }
    return i;
}

int calcBestView(grid trees)
{
    int score = 1;
    int maxscore = 0;
    for (int y = 0; y < GRIDSIZE; ++y) {
        for (int x = 0; x < GRIDSIZE; ++x) {
            for (dir d = TOP; d <= RIGHT; ++d) {
                score = score * getDist(trees, x, y, d);
            }
            if (score > maxscore) {
                maxscore = score;
            }
            score = 1;
        }
    }
    return maxscore;
}

bool heightCheck(grid trees, bgrid vis, int x, int y, int* height)
{
    if ((*height == 9)) {
        return false;
    } else {
        if ((*trees)[y][x] > *height) {
            *height = (*trees)[y][x];
            (*vis)[y][x] = true; // Tree Visible
        }
        return true;
    }
}

void calcVis(grid trees, bgrid vis)
{
    int h = -1;
    for (int y = 0; y < GRIDSIZE; ++y) {
        for (int x = 0; x < GRIDSIZE; ++x) {
            // LEFT PASS
            (*vis)[y][x] = false;
            if (!heightCheck(trees, vis, x, y, &h))
                break;
        }
        h = -1;
        for (int x = (GRIDSIZE - 1); x >= 0; --x) {
            // RIGHT PASS
            if (!heightCheck(trees, vis, x, y, &h))
                break;
        }
        h = -1;
    }
    for (int x = 0; x < GRIDSIZE; ++x) {
        for (int y = 0; y < GRIDSIZE; ++y) {
            // TOP PASS
            if (!heightCheck(trees, vis, x, y, &h))
                break;
        }
        h = -1;
        for (int y = (GRIDSIZE - 1); y >= 0; --y) {
            // BOTTOM PASS
            if (!heightCheck(trees, vis, x, y, &h))
                break;
        }
        h = -1;
    }
}

/// Day Eight of the Advent of Code in C
void main(int argc, char* argv[])
{
    grid trees = malloc(GRIDSIZE * GRIDSIZE * sizeof(int));
    bgrid vis = malloc(GRIDSIZE * GRIDSIZE * sizeof(bool));
    char c = '\0';
    int x = 0;
    int y = 0;
    int count = 0;
    clock_t t = clock();

    if (argc > 0) {
        char* file = argv[1];
        FILE* fp = fopen(file, "r");
        while ((c = fgetc(fp)) != EOF) {
            if (x >= GRIDSIZE || c == '\n') {
                y++;
                x = 0;
            } else {
                (*trees)[y][x] = (int)(c - '0');
                x++;
            }
        }
        calcVis(trees, vis);

        for (int y = 0; y < GRIDSIZE; y++) {
            for (int x = 0; x < GRIDSIZE; x++) {
                if ((*vis)[y][x]) {
                    count++;
                }
            }
        }
        printf("Visible Trees: %d\n", count);
        printf("Best View Score:%d\n", calcBestView(trees));
        printf("Time elapsed: %f", (double)(clock() - t) / CLOCKS_PER_SEC);
        fclose(fp);
        free(fp);
        free(trees);
        free(vis);
    }
}