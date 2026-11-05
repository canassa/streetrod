/* FAME.H - declarations of what FAME.C defines. */

#ifndef FAME_H
#define FAME_H

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

#endif
