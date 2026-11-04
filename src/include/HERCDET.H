/* HERCDET.H - declarations of what HERCDET.ASM defines. */

#ifndef HERCDET_H
#define HERCDET_H
#include "STRUCTS.H"

extern unsigned char g_5580[543];                       /* DS:5580 */
extern struct KeyMap *g_keyMaps[];                      /* DS:579F */
void hercdet_ram_test(void);                            /* 0x0E56A */
int hercdet_detect(void);                               /* 0x0E584 */
int hercdet_bios_mono(void);                            /* 0x0E5C3 */

#endif
