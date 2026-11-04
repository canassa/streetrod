/* ADS.H - declarations of what ADS.C defines. */

#ifndef ADS_H
#define ADS_H
#include "STRUCTS.H"

extern struct Blt g_adPacket; /* DS:0522 */

extern struct Pages g_pages;                            /* DS:04FE */
extern struct Bitmap far *g_adPicture;                  /* DS:051C */
extern char g_adRedraw;                                 /* DS:0520 */
extern int g_adResume;                                  /* DS:0536 */
extern int g_adIdxEndPageState;                                      /* DS:7F84 */
void ads_open(int *list, int base);                     /* 0x008B4 */
int ads_run(void);                                      /* 0x009CB */
void ads_close(void);                                   /* 0x00D58 */

#endif
