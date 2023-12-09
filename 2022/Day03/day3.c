#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INPUTSIZE 6

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
    char line[150];
    int len = 0;
    int sum = 0;
    char* file = argv[1];
    FILE* fp = fopen(file, "r");
    while (fgets(line, 150, fp)) {
        len = strlen(line);
        // printf("len: %d\n",len);
        for (int i = 0; i < (len / 2); ++i) {
            for (int j = (len / 2); j < len; ++j) {
                if (line[i] == line[j]) {
                    printf("%c: %d\n", line[i], charPrio(line[i]));
                    sum += charPrio(line[i]);
                    i = len; // break for
                }
            }
        }
        if (feof(fp))
            break;
    }
    printf("%d", sum);
    fclose(fp);
    free(fp);
}