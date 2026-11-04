/* ADS.H - declarations of what ADS.C defines. */

#ifndef ADS_H
#define ADS_H
#include "STRUCTS.H"

extern struct Pages g_pages;                            /* DS:04FE */
extern struct Bitmap far *g_adPicture;                  /* DS:051C */
extern char g_adRedraw;                                 /* DS:0520 */
extern int g_adBlt;                                     /* DS:0522 */
extern int g_adHeight;                                      /* DS:0524 */
extern int g_adSrcX;                                      /* DS:0526 */
extern int g_adSrcY;                                      /* DS:0528 */
extern int g_adDstX;                                      /* DS:052A */
extern int g_adDstY;                                      /* DS:052C */
extern int g_052e;                                      /* DS:052E */
extern int g_0530[3];                                   /* DS:0530 */
extern int g_adResume;                                  /* DS:0536 */
extern int g_adIdxEndPageState;                                      /* DS:7F84 */
void ads_open(int *list, int base);                     /* 0x008B4 */
int ads_run(void);                                      /* 0x009CB */
void ads_close(void);                                   /* 0x00D58 */

#endif
