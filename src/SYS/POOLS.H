/* POOLS.H - declarations of what POOLS.C defines. */

#ifndef POOLS_H
#define POOLS_H
#include "STRUCTS.H"

extern struct Node2 g_storageCarAllocTable[16];                         /* DS:83B0 */
extern struct Node g_allocationNodesPartList[140];                        /* DS:866A */
void near pl_init(int reset);                        /* 0x03614 */

#endif
