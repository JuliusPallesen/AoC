#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int charPrio(char c)
{
    if ((c - 'a') < 0) {
        return (int)(c - 'A') + 27;
    } else {
        return (int)(c - 'a') + 1;
    }
}

/// Day three of the Advent of Code in C
void main(int argc, char* argv[])
{
    char lines[3][150];
    int sum = 0;
    int i = 0;
    char* file = argv[1];
    FILE* fp = fopen(file, "r");
    while (fgets(lines[i % 3], 150, fp)) {
        if ((i > 0) && (((i + 1) % 3) == 0)) {
            for (int j = 0; j < strlen(lines[0]); ++j) {
                char c = lines[0][j];
                if ((strchr(lines[1], c) != NULL) && (strchr(lines[2], c) != NULL)) {
                    printf("%c: %d\n", c, charPrio(c));
                    sum += charPrio(c);
                    j = strlen(lines[0]);
                }
            }
        }
        char lines[3][150];
        i++;
        if (feof(fp))
            break;
    }
    printf("%d", sum);
    fclose(fp);
    free(fp);
}