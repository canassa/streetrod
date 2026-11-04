#include "POOLS.H"
#include "SCREEN.H"

struct Node2 g_storageCarAllocTable[16];
struct Node g_allocationNodesPartList[140];


void near pl_init(int reset)
{
    struct Node2 *p2;
    struct Node *r;
    int i;

    for (i = 0, p2 = g_driver.freeCars = g_storageCarAllocTable; i < 16; i++) {
        p2->next = p2 + 1;
        p2->b = -1;
        p2++;
    }
    p2--;
    p2->next = 0;
    for (i = 0, r = g_driver.freeParts = g_allocationNodesPartList; i < 140; i++) {
        r->next = r + 1;
        r->idx = -1;
        r++;
    }
    r--;
    r->next = 0;
    g_driver.carCount = g_driver.partCount = 0;
    if (reset) {
        g_driver.currentCar = 0;
        g_driver.otherCars = 0;
        g_driver.spareParts = 0;
        g_driver.money = 0;
        g_driver.unused = 10;
    }
}
