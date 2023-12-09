#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INPUTSIZE 2773
#define NAME_LEN 5
#define PLUS 0
#define MINUS 1
#define MULT 2
#define DIV 3
#define LEFT 0
#define RIGHT 1

typedef struct tree {
    char name[NAME_LEN];
    double value;
    struct tree* left;
    struct tree* right;
} tree;

typedef struct hashEntry {
    int size;
    struct tree* entries[20];
} hashEntry;

typedef struct listener {
    char name[NAME_LEN];
    int lsize;
    int rsize;
    struct tree* listenleft[20];
    struct tree* listenright[20];
} listener;

void printTree(struct tree* monkey)
{
    printf("[name: %s\n", monkey->name);
    printf(" val: %f\n", monkey->value);

    printf(" left: ");
    if (monkey->left != NULL)
        printf((char*)(monkey->left->name));
    printf(" right: ");
    if (monkey->right != NULL)
        printf((char*)(monkey->right->name));
    printf("]\n");
}

double res(struct tree* monkey)
{
    if (strcmp(monkey->name, "humn") == 0) {
        return monkey->value;
    }
    double left = 0;
    if (monkey->left->name == NULL && monkey->right->name == NULL) {
        return monkey->value;
    } else {

        left = res(monkey->left);
        double right = 0;
        switch ((int)monkey->value) {
        case PLUS:
            right = res(monkey->right);
            return left + right;
        case MINUS:
            right = res(monkey->right);
            return left - right;
        case MULT:
            right = res(monkey->right);
            return left * right;
        case DIV:
            right = res(monkey->right);
            return left / right;
        default:
            return 0.0;
        }
    }
    return 0.0;
}

int opToInt(char op)
{
    switch (op) {
    case '+':
        return PLUS;
    case '-':
        return MINUS;
    case '*':
        return MULT;
    case '/':
        return DIV;
    default:
        return -1;
    }
}

int hashFun(char* input)
{
    int hash = 5381;
    int c;
    int i = 0;
    while (c = *input++) {
        hash = ((hash << 5) + hash) + c;
        if (i >= NAME_LEN)
            break;
        i++;
    }
    return hash % INPUTSIZE;
}

void printHashTable(struct hashEntry* tbl)
{
    for (int i = 0; i < INPUTSIZE; ++i) {
        printf("%d [size: %d]: ", i, tbl[i].size);
        for (int j = 0; j < tbl[i].size; ++j) {
            printf("%s, ", tbl[i].entries[j]->name);
        }
        printf("\n");
    }
}

void addHash(struct hashEntry* tbl, struct tree* mon)
{
    int hash = hashFun(mon->name);
    tbl[hash].entries[tbl[hash].size] = mon;
    tbl[hash].size = 1 + tbl[hash].size;
}

// returns if a monkey is already in the hashtable
bool isInHash(struct hashEntry* tbl, char* name)
{
    int hash = hashFun(name);
    if (tbl[hash].entries != NULL) {
        for (int i = 0; i < tbl[hash].size; ++i) {
            if (strcmp(tbl[hash].entries[i]->name, name) == 0)
                return true;
        }
    }
    return false;
}

tree* getElemHash(struct hashEntry* tbl, char* name)
{
    for (int i = 0; i < tbl[hashFun(name)].size; ++i) {
        if (strcmp(tbl[hashFun(name)].entries[i]->name, name) == 0) {
            return tbl[hashFun(name)].entries[i];
        }
    }
    return NULL;
}

int getIndex(struct listener* l, char* name, int size)
{
    for (int i = 0; i < size; ++i) {
        if (strcmp(l[i].name, name) == 0)
            return i;
    }
    return -1;
}

void initHashTable(struct hashEntry* h)
{
    for (int i = 0; i < INPUTSIZE; ++i) {
        hashEntry* e = (hashEntry*)malloc(sizeof(hashEntry));
        e->size = 0;
        h[i] = *e;
    }
}

void inittListeners(struct listener* l)
{
    for (int i = 0; i < INPUTSIZE; ++i) {
        struct listener e;
        e.lsize = 0;
        e.rsize = 0;
        l[i] = e;
    }
}

void addListener(struct listener* l, char* name, struct tree* mon, int* size, int side)
{
    int index = getIndex(l, name, *size);
    if (index <= 0) {
        *size = *size + 1;
        strcpy(l[*size - 1].name, name);
        if (side == LEFT) {
            l[*size - 1].listenleft[0] = mon;
            l[*size - 1].lsize = 1;
        } else {
            l[*size - 1].listenright[0] = mon;
            l[*size - 1].rsize = 1;
        }
    } else {
        struct listener elem = l[index];
        if (side == LEFT) {
            elem.lsize++;
            elem.listenleft[(elem.lsize) - 1] = mon;
        } else {
            elem.rsize++;
            elem.listenright[(elem.rsize) - 1] = mon;
        }
    }
}

// Notifies all monkeys who listen to the newly added monkey
void notifyListeners(struct listener* l, struct tree* mon, int size)
{
    int index = getIndex(l, mon->name, size);
    if (index >= 0) {
        for (int i = 0; i < l[index].lsize; i++) {
            if (0 == strcmp(l[index].listenleft[i]->name, ""))
                break;
            l[index].listenleft[i]->left = mon;
        }
        for (int i = 0; i < l[index].rsize; i++) {
            if (0 == strcmp(l[index].listenright[i]->name, ""))
                break;
            l[index].listenright[i]->right = mon;
        }
    }
}

/// Day Twentyfirst of the Advent of Code in C
int main(int argc, char* argv[])
{
    char line[150];
    int num = 0;
    int listenerSize = 0;
    char left[NAME_LEN];
    char right[NAME_LEN];
    char name[NAME_LEN];
    char op;
    struct hashEntry hashTable[INPUTSIZE];
    initHashTable(hashTable);
    struct listener listenerTable[INPUTSIZE];
    inittListeners(listenerTable);
    struct tree* root = malloc(sizeof(tree));
    if (argc < 1) {
        exit(1);
    }
    char* file = argv[1];
    FILE* fp = fopen(file, "r");
    while (fgets(line, 150, fp)) {
        struct tree* newElem = malloc(sizeof(tree));
        newElem->left = NULL;
        newElem->right = NULL;
        if (sscanf(line, "%[^:]: %d\n", name, &num) == 2) {
            strcpy((char*)newElem->name, name);
            newElem->value = (double)num;
        } else if (sscanf(line, "%[^:]: %4s %c %[^\n]\n", name, left, &op, right)) {
            strcpy((char*)(newElem->name), name);
            newElem->value = (double)opToInt(op);
            isInHash(hashTable, left) ? newElem->left = getElemHash(hashTable, left) : addListener(listenerTable, left, newElem, &listenerSize, LEFT);
            isInHash(hashTable, right) ? newElem->right = getElemHash(hashTable, right) : addListener(listenerTable, right, newElem, &listenerSize, RIGHT);
        } else {
            printf("Could not read line:%s", line);
        }
        addHash(hashTable, newElem);
        notifyListeners(listenerTable, newElem, listenerSize);
        if (feof(fp))
            break;
    }
    root = getElemHash(hashTable, "root");
    struct tree* humn = getElemHash(hashTable, "humn");
    printf("\nResult:\n");
    printf("%f\n", res(root));
    printf("\nleft:\n");
    printf("%f\n", res(root->left));
    printf("\nright: \n");
    printf("%f\n", res(root->right));
    free(hashTable);
    free(listenerTable);
    fclose(fp);
}