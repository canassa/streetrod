/* SAVEGAME.H - declarations of what SAVEGAME.C defines. */

#ifndef SAVEGAME_H
#define SAVEGAME_H
#include "STRUCTS.H"

extern char g_noNameMsg[];                              /* DS:4FA6 */
extern char g_saveSlots[];                              /* DS:4FC2 */
extern int g_slotNameOfs[];                             /* DS:4FD2 */
extern char *g_noNameMsgPtr;                            /* DS:4FF2 */
extern int g_demoRacer;                                 /* DS:4FF4 */
extern char g_501d;                                     /* DS:501D */
extern char g_buyCarMsg[];                              /* DS:501E */
void sg_init_racer(int k, int *list);             /* 0x0542E */
int near sg_new(void);                            /* 0x0570C */
int near sg_load(void);                           /* 0x05B4A */
void near sg_save(void);                          /* 0x05EEA */
int near sg_menu(void);                           /* 0x0641E */

#endif
