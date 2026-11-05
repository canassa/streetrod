/**
 * @file MAIN.H
 * @brief Startup video selection and current scene state.
 */

#ifndef MAIN_H
#define MAIN_H

#include "VIDEO.H"

struct Bitmap;

/** @brief Location index for car placement and background selection. */
enum Scene {
    SCENE_NONE = 0,        /**< No location-specific car backdrop. */
    SCENE_GARAGE = 1,      /**< Use the home garage car backdrop. */
    SCENE_BOBS = 2,        /**< Use the diner car backdrop. */
    SCENE_GAS_STATION = 3, /**< Use the pump foreground and car backdrop. */
    SCENE_ADVERTS = 5,     /**< Use the classified-ad car preview backdrop. */
    SCENE_JAIL = 6         /**< Police backdrop restricts car drawing. */
};

/**
 * @name Hardware bitmap formats
 * @brief Negative display tags used in bitmap flags and video dispatch.
 * @{
 */
#define VID_EGA (-2)
#define VID_CGA (-3)
#define VID_HERCULES (-4)
#define VID_TANDY (-6)
/** @} */

/** @brief Startup adapter choices before conversion to a drawing format. */
enum VideoChoice {
    VIDEO_CHOICE_CGA = 1,       /**< Four-color assets and CGA video memory. */
    VIDEO_CHOICE_EGA = 2,       /**< Planar assets and EGA pages. */
    VIDEO_CHOICE_VGA = 3,       /**< EGA drawing with VGA palette handling. */
    VIDEO_CHOICE_HERCULES = 4,  /**< CGA assets in monochrome memory. */
    VIDEO_CHOICE_TANDY = 5,     /**< Packed assets in Tandy video memory. */
    VIDEO_CHOICE_CGA_ON_EGA = 6 /**< CGA assets with EGA palette setup. */
};

extern int g_hercules;
extern int g_sysHooked;
extern int g_videoSet;
extern int g_dashViewSaved;
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
