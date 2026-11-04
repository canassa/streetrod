/* FAME.H - declarations of what FAME.C defines. */

#ifndef FAME_H
#define FAME_H
#include "STRUCTS.H"

extern struct ManualEntry g_manual[];                   /* DS:4B62 */
extern int g_4bc8;                                      /* DS:4BC8 */
extern int g_4bca[];                                    /* DS:4BCA */
extern struct Colors g_4be2;                            /* DS:4BE2 */
extern struct Colors g_4be8;                            /* DS:4BE8 */
extern char g_4bed;                                     /* DS:4BED */
extern int bucount;                                     /* DS:7646 */
extern char atnewName[];                                /* DS:7E9A */
extern struct Entry aytablex[];                         /* DS:82CA */
int fame_protect_check(void);                           /* 0x02FC8 */
int fame_end_menu(void);                                /* 0x0315B */
void fame_show(int add);                                /* 0x0339F */

#endif
