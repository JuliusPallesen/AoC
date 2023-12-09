#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define INPUTSIZE 5000

typedef struct listElem {
    struct listElem* next;
    struct listElem* prev;
    int value;
    bool moved;
} listElem;

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
    if (pos <= elem->value) {
        moveFwd(list->next, elem, pos + 1);
    } else {
        insertElem(list, elem);
    }
}

void moveBwd(struct listElem* list, struct listElem* elem, int pos)
{
    if (pos > elem->value) {
        moveBwd(list->prev, elem, pos - 1);
    } else {
        insertElem(list, elem);
    }
}

void encryptElem(struct listElem* elem)
{
    int i = elem->value;
    elem->moved = true;
    if (i < 0) {
        moveBwd(elem, elem, 0);
    } else {
        moveFwd(elem, elem, 0);
    }
}

struct listElem* findStart(struct listElem* ptr, int i)
{
    if (i == 0)
        return ptr;
    if (i > 0) {
        findStart(ptr->prev, i - 1);
    } else {
        findStart(ptr->next, i + 1);
    }
}

int calcGroveSum(struct listElem* list)
{
    int zeroPos = INPUTSIZE;
    int i = 0;
    int sum = 0;
    int grove = 0;
    int found = 0;
    while (found < 3) {
        if ((list->value == 0) && zeroPos == INPUTSIZE) {
            zeroPos = i;
            printf("Zero at: %d\n", i);
            i = 0;
        }
        if ((i == 1000 || i == 2000 || i == 3000)
            && (zeroPos != INPUTSIZE)) {
            printf("%d->%d, ", i, list->value);
            sum = sum + list->value;
            found++;
        }
        list = list->next;
        i++;
    }
    return sum;
}

/// Day Twenty of the Advent of Code in C
void main(int argc, char* argv[])
{
    char line[150];
    int num = 0;
    int i = 0;
    int movedElems = 0;
    bool firstFound = false;
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
        newElem->value = num;
        append(last, newElem);
        if (!firstFound) {
            first = newElem;
            firstFound = true;
        }
        last = newElem;
        if (feof(fp))
            break;
    }
    last->next = first;
    first->prev = last;
    ptr = first;
    printf("\nEncrypting...\n");
    while (movedElems < INPUTSIZE) {
        if (!(ptr->moved)) {
            next = ptr->next;
            movedElems++;
            ptr->moved = true;
            if (ptr->value != 0) {
                rmElem(ptr);
                encryptElem(ptr);
            }
            ptr = next->prev;
        }
        ptr = ptr->next;
    }
    printf("\nResult:\n");
    printf("Grove Number: %d \n", calcGroveSum(first));
    fclose(fp);
}