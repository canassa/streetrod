/* ROAD.H - declarations of what ROAD.C defines. */

#ifndef ROAD_H
#define ROAD_H
#include "STRUCTS.H"

extern int g_yBuildingTopArray[15];

extern struct MirrorEdges g_mirGeom;

extern unsigned far *g_rightSideTerrainRow;
extern int g_mirYProj[8];

extern int g_hillProfile[];
extern int g_hillDipProfile[];
extern int g_bumpProfile[17];
extern int g_layoutLen;
extern int g_roadLayouts[][12];
extern struct Bitmap g_egaPage1;
extern struct Bitmap g_egaPage2;
extern int g_roadLayout;
extern int g_onShoulder;
extern int g_turnRight;
extern int g_turnLeft;
extern unsigned far *q_72ee;
extern int g_roadLXArray[15];
extern int far *g_hereCurvePtr;
extern int far *af7880z;
extern int g_roadYProjectionArray[15];
extern int aw7aaex;
extern int g_rightXProjectionTable[15];
extern unsigned far *g7d62p;
extern int g_lhsRdMirrorX[8];
extern int g_roadOuterLeftXArray[15];
extern int g_rhsOuterXArray[15];
void rd_build_layout(int n);
void rd_generate(void);
void rd_seek(int n);
int rd_proj_x(int a, int b, int c);
int rd_proj_y(int a, int b, int c);
int rd_mirror_x(int a, int b, int c);
int rd_mirror_y(int a, int b, int c);
void rd_reset(void);
void rd_project_ahead(void);
void rd_project_behind(void);
void rd_draw_frame(void);
void rd_advance(void);
int rd_check_crash(void);
void rd_start(void);
void rd_turn(int dir);
void rd_update(void);

#endif
