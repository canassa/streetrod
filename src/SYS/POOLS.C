#include "POOLS.H"
#include "SCREEN.H"

struct CarPoolNode g_storageCarAllocTable[CAR_NODE_COUNT];
struct PartNode g_allocationNodesPartList[PART_NODE_COUNT];


void near pl_init(int reset)
{
    struct CarPoolNode *p2;
    struct PartNode *r;
    int i;

    for (i = 0, p2 = g_driver.freeCars = g_storageCarAllocTable; i < CAR_NODE_COUNT; i++) {
        p2->next = p2 + 1;
        p2->specIndex = POOL_ENTRY_FREE;
        p2++;
    }
    p2--;
    p2->next = 0;
    for (i = 0, r = g_driver.freeParts = g_allocationNodesPartList; i < PART_NODE_COUNT; i++) {
        r->next = r + 1;
        r->catalogIndex = POOL_ENTRY_FREE;
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
