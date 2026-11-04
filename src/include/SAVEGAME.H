/* SAVEGAME.H - declarations of what SAVEGAME.C defines. */

#ifndef SAVEGAME_H
#define SAVEGAME_H
#include "STRUCTS.H"

extern char g_4fa6[];                                   /* DS:4FA6 */
extern char g_4fc2[];                                   /* DS:4FC2 */
extern int g_4fd2[];                                    /* DS:4FD2 */
extern char *g_4ff2;                                    /* DS:4FF2 */
extern int g_4ff4;                                      /* DS:4FF4 */
extern char g_501d;                                     /* DS:501D */
extern char g_501e[];                                   /* DS:501E */
void savegame_init_racer(int k, int *list);             /* 0x0542E */
int near savegame_new(void);                            /* 0x0570C */
int near savegame_load(void);                           /* 0x05B4A */
void near savegame_save(void);                          /* 0x05EEA */
int near savegame_menu(void);                           /* 0x0641E */

#endif
