#include <stdio.h>
#include <stdlib.h>

/// Day One of the Advent of Code in C
void main(int argc, char* argv[])
{
    char line[150];
    int len = 0;
    int sum = 0;
    int num = 0;
    int elfs = 0;
    int max = 0;
    if (argc > 0) {
        char* file = argv[1];
        FILE* fp = fopen(file, "r");
        while (fgets(line, 150, fp) && !feof(fp)) {
            sscanf(line, "%d/n", &num);
            if (num != 0) {
                sum += num;
            } else {
                elfs++;
                printf("Elf %d has %d calories\n", elfs, sum);
                if (sum > max) {
                    max = sum;
                }
                sum = 0;
            }
            num = 0;
        }
        if (sum > max) {
            max = sum;
        }
        printf("Most calories: %d", max);
        fclose(fp);
        free(fp);
    } else {
        printf("Please provide a filepath");
    }
}