#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define INPUTSIZE 7
#define KEY 811589153
#define ITER 10

typedef struct listElem {
    struct listElem* next;
    struct listElem* prev;
    long long value;
    bool moved;
} listElem;

void printList(struct listElem* list)
{
    for (int i = 0; i < INPUTSIZE; ++i) {
        list = list->next;
    }
}

void append(struct listElem* end, struct listElem* new)
{
    end->next = new;
    new->prev = end;
}

void rmElem(struct listElem* elem)
{
    elem->prev->next = elem->next;
    elem->next->prev = elem->prev;
}

void insertElem(struct listElem* list, struct listElem* elem)
{
    list->prev->next = elem;
    elem->prev = list->prev;
    list->prev = elem;
    elem->next = list;
}

void moveFwd(struct listElem* list, struct listElem* elem, int pos)
{
    if (pos <= ((elem->value) % INPUTSIZE)) {
        moveFwd(list->next, elem, pos + 1);
    } else {
        insertElem(list, elem);
    }
}

void moveBwd(struct listElem* list, struct listElem* elem, int pos)
{
    if (pos > ((elem->value) % INPUTSIZE)) {
        moveBwd(list->prev, elem, pos - 1);
    } else {
        insertElem(list, elem);
    }
}

void encryptElem(struct listElem* elem)
{
    elem->moved = true;
    if (elem->value < 0) {
        moveBwd(elem, elem, 0);
    } else {
        moveFwd(elem, elem, 0);
    }
}

long long calcGroveSum(struct listElem* list)
{
    int zeroPos = INPUTSIZE;
    int i = 0;
    long long sum = 0;
    long long grove = 0;
    int found = 0;
    while (found < 3) {
        if ((list->value == 0) && zeroPos == INPUTSIZE) {
            zeroPos = i;
            i = 0;
        }
        if ((i == 1000 || i == 2000 || i == 3000)
            && (zeroPos != INPUTSIZE)) {
            sum = sum + list->value;
            found++;
        }
        list = list->next;
        i++;
    }
    return sum;
}

/// Day Twenty of the Advent of Code in C
void main(long long argc, char* argv[])
{
    char line[150];
    int num = 0;
    int i = 0;
    bool firstFound = false;
    struct listElem* ord[INPUTSIZE];
    struct listElem* last = malloc(sizeof(listElem));
    struct listElem* first = malloc(sizeof(listElem));
    struct listElem* ptr = malloc(sizeof(listElem));
    struct listElem* next = malloc(sizeof(listElem));
    if (argc < 1) {
        exit(1);
    }
    char* file = argv[1];
    FILE* fp = fopen(file, "r");
    while (fgets(line, 150, fp)) {
        sscanf(line, "%d/n", &num);
        struct listElem* newElem = malloc(sizeof(listElem));
        newElem->moved = false;
        newElem->value = (long long)num * KEY;
        append(last, newElem);
        ord[i] = newElem;
        if (!firstFound) {
            first = newElem;
            firstFound = true;
        }
        last = newElem;
        i++;
        if (feof(fp))
            break;
    }
    last->next = first;
    first->prev = last;
    ptr = first;
    for (int j = 0; j < ITER; ++j) {
        printf("%d:  ", j);
        for (int k = 0; k < INPUTSIZE; k++) {
            rmElem(ord[k]);
            encryptElem(ord[k]);
        }
    }
    printf("Grove Number: %lld \n", calcGroveSum(first));
    fclose(fp);
}