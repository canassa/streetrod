/* FAME.H - declarations of what FAME.C defines. */

#ifndef FAME_H
#define FAME_H
#include "STRUCTS.H"

extern struct ManualEntry g_manual[];                   /* DS:4B62 */
extern int g_protectCycle;                              /* DS:4BC8 */
extern int g_fameRowY[];                                /* DS:4BCA */
extern struct Colors g_fameColors1;                     /* DS:4BE2 */
extern struct Colors g_fameColors2;                     /* DS:4BE8 */
extern char g_4bed;                                     /* DS:4BED */
extern int g_nScore;                                     /* DS:7646 */
extern char g_driverName[];                             /* DS:7E9A */
extern struct Entry g_hallFameTbl[];                         /* DS:82CA */
int fame_protect_check(void);                           /* 0x02FC8 */
int fame_end_menu(void);                                /* 0x0315B */
void fame_show(int add);                                /* 0x0339F */

#endif
