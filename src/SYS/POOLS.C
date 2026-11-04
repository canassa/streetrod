#include "STRUCTS.H"
#include "POOLS.H"
#include "SCREEN.H"

/* Uninitialized storage: communals, allocated by LINK (DS:6E60..8F02); names chosen to fit LINK's hash buckets (docs/RECONSTRUCTION.md). */
struct Node2 g_storageCarAllocTable[16];
struct Node g_allocationNodesPartList[140];

void near pl_init(int reset)
{
    struct Node2 *p2;
    struct Node *r;
    int i;

    for (i = 0, p2 = a_7eaap.freeCars = g_storageCarAllocTable; i < 16; i++) {
        p2->next = p2 + 1;
        p2->b = -1;
        p2++;
    }
    p2--;
    p2->next = 0;
    for (i = 0, r = a_7eaap.freeParts = g_allocationNodesPartList; i < 140; i++) {
        r->next = r + 1;
        r->idx = -1;
        r++;
    }
    r--;
    r->next = 0;
    a_7eaap.carCount = a_7eaap.partCount = 0;
    if (reset) {
        a_7eaap.currentCar = 0;
        a_7eaap.otherCars = 0;
        a_7eaap.spareParts = 0;
        a_7eaap.money = 0;
        a_7eaap.m7eae = 10;
    }
}
