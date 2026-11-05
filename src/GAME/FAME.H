/* FAME.H - declarations of what FAME.C defines. */

#ifndef FAME_H
#define FAME_H

#define HALL_FAME_COUNT 10
#define MANUAL_ENTRY_COUNT 51
#define MANUAL_ICON_COUNT 0x12

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

enum ManualKind {
    MANUAL_CAR_KEY = 0,
    MANUAL_CAR_ICON = 1
};

#endif
