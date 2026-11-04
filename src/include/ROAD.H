/* ROAD.H - declarations of what ROAD.C defines. */

#ifndef ROAD_H
#define ROAD_H
#include "STRUCTS.H"

extern struct BuildingRows g_currentBuildingRowsInfo; /* DS:7AB0 */

extern struct MirrorEdges g_videoMirrorEdgesSet; /* DS:864A */

extern struct MirrorRoad g_sharedMirrorRoadSet; /* DS:8634 */

extern int g_hillProfile[];                             /* DS:5C20 */
extern int g_hillDipProfile[];                          /* DS:5C4C */
extern int g_bumpProfile[17];                           /* DS:5C98 */
extern int g_layoutLen;                                 /* DS:5CBA */
extern int g_roadLayouts[][12];                         /* DS:5CBC */
extern struct Bitmap g_egaPage1;                        /* DS:5DAC */
extern struct Bitmap g_egaPage2;                        /* DS:5DDC */
extern int g_roadLayout;                                /* DS:5E0C */
extern int g_onShoulder;                                /* DS:5E0E */
extern int g_turnRight;                                 /* DS:5E10 */
extern int g_turnLeft;                                  /* DS:5E12 */
extern unsigned far *q_72ee;                            /* DS:72EE */
extern int g_roadLXArray[];                                    /* DS:72F8 */
extern int far *g_hereCurvePtr;                                /* DS:7560 */
extern int far *af7880z;                                /* DS:7880 */
extern int bq7884[];                                    /* DS:7884 */
extern int aw7aaex;                                     /* DS:7AAE */
extern int g_rightXProjectionTable[];                                    /* DS:7ACE */
extern unsigned far *g7d62p;                            /* DS:7D62 */
extern int g_lhsRdMirrorX[];                                    /* DS:7F22 */
extern int g_xOuterLProjArray[];                                   /* DS:7F8A */
extern int g_rhsOuterXArray[];                                    /* DS:7FD8 */
void road_build_layout(int n);                          /* 0x26FE1 */
void road_generate(void);                               /* 0x27198 */
void road_seek(int n);                                  /* 0x27B6D */
int road_proj_x(int a, int b, int c);                   /* 0x27BBF */
int road_proj_y(int a, int b, int c);                   /* 0x27C01 */
int road_mirror_x(int a, int b, int c);                 /* 0x27C3D */
int road_mirror_y(int a, int b, int c);                 /* 0x27C78 */
void road_reset(void);                                  /* 0x27CAD */
void road_project_ahead(void);                          /* 0x27D4B */
void road_project_behind(void);                         /* 0x2801C */
void road_draw_frame(void);                             /* 0x282EE */
void road_advance(void);                                /* 0x2834C */
int road_check_crash(void);                             /* 0x284BB */
void road_start(void);                                  /* 0x28564 */
void road_turn(int dir);                                /* 0x28576 */
void road_update(void);                                 /* 0x2858D */

#endif
