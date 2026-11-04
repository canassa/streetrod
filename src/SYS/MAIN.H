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

extern int g_hercules;
extern int g_sysHooked;
extern int g_hercModeSet;
extern int g_videoSet;
extern int g_004a;
extern int g_004c;
extern char *g_videoMenu[];
extern int g_egaCard;
extern int g_forceEga;
extern int g_vga;
extern int g_useMouse;
extern int g_noUeMem;
extern int g_end;
extern int g_scene;
extern int g_carAnim;
extern struct Bitmap far *g_spritePic;
extern struct Bitmap far *g_pumpWithNozzle;
extern struct Bitmap far *g_pumpNoNozzle;
extern struct Bitmap far *g_stickerBg;
extern int g_carBandHeights[7];
extern int g_carBottomCuts[7];
extern int g_carBg4[7];
extern int g_carBg16[7];
extern int g_spriteBottomCut;
extern int g_loadedIntroBmps;
extern struct Bitmap far *g_bufBackgroundDashPtr;
extern char far *g_imageBufferPointer;
extern struct Video r_vid;
extern char g_graphicsUp;
extern int g_hasSlowCpu;
extern int bu8acc;
extern int g_attractSwitchStartup;
void mi_set_mode(int ax);
void mi_set_palette(unsigned char color);

#endif
