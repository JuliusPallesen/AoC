#include <stdio.h>
#include <stdlib.h>

/// Day Four of the Advent of Code in C
void main(int argc, char* argv[])
{
    char line[150];
    int sum = 0;
    int s1, s2, e1, e2;
    char* file = argv[1];
    FILE* fp = fopen(file, "r");
    while (fgets(line, 150, fp)) {
        sscanf(line, "%d-%d,%d-%d\n", &s1, &e1, &s2, &e2);
        if (((s1 <= s2) && (s2 <= e1)) || ((s1 <= e2) && (e2 <= e1)) || ((s2 <= s1) && (s1 <= e2)) || ((s2 <= e1) && (e1 <= e2))) {
            sum++;
        }
        if (feof(fp)){
            break;
        }
    }
    printf("%d\n", sum);
    fclose(fp);
    free(fp);
}