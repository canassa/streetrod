/**
 * @file POOLS.H
 * @brief Fixed car and spare-part storage pools.
 */

#ifndef POOLS_H
#define POOLS_H

/**
 * @name Garage storage capacities
 * @brief Entries in the car and spare-part free lists, including owned items.
 * @{
 */
#define CAR_NODE_COUNT 16
#define PART_NODE_COUNT 140
/** @} */

/** Index marking an unallocated car or part pool entry. */
#define POOL_ENTRY_FREE (-1)

/**
 * @brief An installed part, spare part or free equipment-pool entry.
 *
 * Save files store the record and rebuild its list links when loading.
 */
struct PartNode {
    int value; /**< Accounted value in dollars, before resale adjustments. */
    /** Wear in hundredths of a percent, or @ref PART_WEAR_UNTRACKED. */
    int wear;
    int catalogIndex;      /**< Parts-catalog index, or @ref POOL_ENTRY_FREE. */
    struct PartNode *next; /**< Spare/free-list link; null when installed. */
};

/**
 * @brief Car-pool storage viewed as a car while allocated.
 *
 * The payload shares the installed-car layout; only the link and free index
 * are used while an entry is on the free list.
 */
struct CarPoolNode {
    int value; /**< Accounted car value in dollars. */
    /** Car specification index, or @ref POOL_ENTRY_FREE. */
    int specIndex;
    char carData[0x22];       /**< Car payload from paint through appearance. */
    struct CarPoolNode *next; /**< Free-list link, sharing the car-list link. */
};

extern struct CarPoolNode g_storageCarAllocTable[CAR_NODE_COUNT];
extern struct PartNode g_allocationNodesPartList[PART_NODE_COUNT];
void near pl_init(int reset);

#endif
