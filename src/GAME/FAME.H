/**
 * @file FAME.H
 * @brief Manual copy protection and hall-of-fame scores.
 */

#ifndef FAME_H
#define FAME_H

/** Maximum number of driver scores retained in the hall of fame. */
#define HALL_FAME_COUNT 10
/**
 * @name Manual question table
 * @brief Select among 51 questions; the first eighteen ask about car icons.
 * @{
 */
#define MANUAL_ENTRY_COUNT 51
#define MANUAL_ICON_COUNT 0x12
/** @} */

#include "VIDEO.H"

struct ManualEntry {
    signed char page;
    signed char color;
};

struct Entry {
    char name[16];
    char score[7];
};

extern int g_protectCycle;
extern int g_nScore;
extern char g_driverName[];
extern struct Entry g_hallFameTbl[];
int fa_protect_check(void);
int fa_end_menu(void);
void fa_show(int add);

/** @brief Copy-protection question selected by the manual-table index. */
enum ManualKind {
    /** Ask the key colour for entries after the icon range. */
    MANUAL_CAR_KEY = 0,
    /** Ask the icon colour for the first eighteen entries. */
    MANUAL_CAR_ICON = 1
};

#endif
