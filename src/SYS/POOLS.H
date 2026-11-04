/* POOLS.H - declarations of what POOLS.C defines. */

#ifndef POOLS_H
#define POOLS_H

struct Node {
    int value;
    int wear;
    int idx;
    struct Node *next;
};

struct Node2 {
    int a;
    int b;
    char pad[0x22];
    struct Node2 *next;
};

extern struct Node2 g_storageCarAllocTable[16];
extern struct Node g_allocationNodesPartList[140];
void near pl_init(int reset);

#endif
