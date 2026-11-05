/* POOLS.H - declarations of what POOLS.C defines. */

#ifndef POOLS_H
#define POOLS_H

#define CAR_NODE_COUNT 16
#define PART_NODE_COUNT 140

struct Node {
    int value;
    int wear;
    int idx;
    struct Node *next;
};

struct Node2 {
    int a;
    int b;
    char carData[0x22];
    struct Node2 *next;
};

extern struct Node2 g_storageCarAllocTable[CAR_NODE_COUNT];
extern struct Node g_allocationNodesPartList[PART_NODE_COUNT];
void near pl_init(int reset);

#endif
