/* MAIN.H - declarations of what MAIN.C defines. */

#ifndef MAIN_H
#define MAIN_H

struct Bitmap;

/* Display state set by vi_init; mode follows front->flags and geometry depends on the mode. */
struct Video {
    struct Bitmap far *front;
    struct Bitmap far *back;
    struct Bitmap far *hoodBg;
    int mode;
    int planes; /* 4 on EGA (-2), else 1 */
    int shift;  /* log2 pixels per byte: 3, 2, 1 */
    int bpp;    /* bits per pixel: 1, 2, 4 */
    int mask;   /* ~(pixels per byte - 1): -8, -4, -2 */
    int copyToFront;
};

enum Scene {
    SCENE_NONE = 0,
    SCENE_GARAGE = 1,
    SCENE_BOBS = 2,
    SCENE_GAS_STATION = 3,
    SCENE_ADVERTS = 5,
    SCENE_JAIL = 6
};

#define VID_EGA (-2)
#define VID_CGA (-3)
#define VID_HERCULES (-4)
#define VID_TANDY (-6)

enum VideoChoice {
    VIDEO_CHOICE_CGA = 1,
    VIDEO_CHOICE_EGA = 2,
    VIDEO_CHOICE_VGA = 3,
    VIDEO_CHOICE_HERCULES = 4,
    VIDEO_CHOICE_TANDY = 5,
    VIDEO_CHOICE_CGA_ON_EGA = 6
};

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
extern enum Scene g_scene;
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
extern int g_processorSpeedRating;
extern int g_attractSwitchStartup;
void mi_set_mode(int ax);
void mi_set_palette(unsigned char color);

#endif
