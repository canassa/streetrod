/* MAIN.H - declarations of what MAIN.C defines. */

#ifndef MAIN_H
#define MAIN_H
#include "STRUCTS.H"

#define VID_EGA (-2)
#define VID_CGA (-3)
#define VID_HERCULES (-4)
#define VID_TANDY (-6)

#define VIDEO_CHOICE_CGA 1
#define VIDEO_CHOICE_EGA 2
#define VIDEO_CHOICE_VGA 3
#define VIDEO_CHOICE_HERCULES 4
#define VIDEO_CHOICE_TANDY 5

extern int g_hercules;                                  /* DS:0042 */
extern int g_sysHooked;                                 /* DS:0044 */
extern int g_0046;                                      /* DS:0046 */
extern int g_videoSet;                                  /* DS:0048 */
extern int g_004a;                                      /* DS:004A */
extern int g_004c;                                      /* DS:004C */
extern char *g_videoMenu[];                             /* DS:0244 */
extern int g_egaCard;                                   /* DS:0250 */
extern int g_forceEga;                                      /* DS:0252 */
extern int g_vga;                                       /* DS:0254 */
extern int g_useMouse;                                  /* DS:0262 */
extern int g_noUeMem;                                   /* DS:0280 */
extern int g_end;                                      /* DS:0282 */
extern int g_scene;                                      /* DS:0284 */
extern int g_carAnim;                                   /* DS:0286 */
extern struct Bitmap far *g_spritePic;                       /* DS:0288 */
extern struct Bitmap far *g_pumpWithNozzle;                       /* DS:028C */
extern struct Bitmap far *g_pumpNoNozzle;                       /* DS:0290 */
extern struct Bitmap far *g_0294;                       /* DS:0294 */
extern int g_carBandHeights[7];                                   /* DS:0298 */
extern int g_carBottomCuts[7];                                   /* DS:02A6 */
extern int g_carBg4[7];                                   /* DS:02B4 */
extern int g_carBg16[7];                                   /* DS:02C2 */
extern int g_spriteBottomCut;                                      /* DS:02D0 */
extern int g_loadedIntroBmps;                                      /* DS:70E2 */
extern struct Bitmap far *g_bufBackgroundDashPtr;                       /* DS:755C */
extern char far *g_imageBufferPointer;                               /* DS:7564 */
extern struct Video r_vid;                              /* DS:822A */
extern char g_graphicsUp;                               /* DS:8247 */
extern int g_hasSlowCpu;                                      /* DS:82C6 */
extern int bu8acc;                                      /* DS:8ACC */
extern int g_attractSwitchStartup;                                     /* DS:8BCE */
void main_set_mode(int ax);                             /* 0x001D1 */
void main_set_palette(unsigned char color);             /* 0x001F2 */

#endif
