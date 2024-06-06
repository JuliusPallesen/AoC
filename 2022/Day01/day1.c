#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define ELFS_AMOUNT 3

size_t get_min_index(int *max_arr, size_t size)
{
    int min = INT_MAX;
    size_t index = 0;
    for (size_t i = 0; i < size; ++i)
    {
        if (min > max_arr[i])
        {
            index = i;
            min = max_arr[i];
        }
    }
    return index;
}

void main(int argc, char *argv[])
{
    if (argc <= 1)
    {
        printf("Please provide a filepath");
        exit(1);
    }
    char line[16];
    int max_arr[ELFS_AMOUNT] = {0, 0, 0};
    size_t min_index = 0;

    int sum = 0;

    char *file = argv[1];
    FILE *fp = fopen(file, "r");
    do
    {
        int num = 0;
        if (sscanf(line, "%d/n", &num) && num != 0)
        {
            sum += num;
            continue;
        }
        if (max_arr[min_index] < sum)
        {
            max_arr[min_index] = sum;
            min_index = get_min_index(max_arr, ELFS_AMOUNT);
        }
        sum = 0;
    } while (fgets(line, 16, fp));

    if (max_arr[min_index] < sum)
    {
        max_arr[min_index] = sum;
        min_index = get_min_index(max_arr, ELFS_AMOUNT);
    }

    int max = 0;
    sum = 0;
    for (size_t i = 0; i < 3; i++)
    {
        if (max_arr[i] > max)
            max = max_arr[i];
        sum += max_arr[i];
    }

    printf("part1: %d\n", max);
    printf("part2: %d\n", sum);
    fclose(fp);
}