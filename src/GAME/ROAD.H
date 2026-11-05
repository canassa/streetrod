/* ROAD.H - declarations of what ROAD.C defines. */

#ifndef ROAD_H
#define ROAD_H

#define ROAD_SURFACE_Y 2000

#include "CARPIC.H"

enum RoadsideFlags {
    ROADSIDE_POST = 0x1,
    ROADSIDE_SMALL_ROCK = 0x4,
    ROADSIDE_LARGE_ROCK = 0x8,
    ROADSIDE_TREE = 0x20,
    ROADSIDE_TREE_3 = 0x80,
    ROADSIDE_BUSH = 0x100,
    ROADSIDE_BUSH_OUTER = 0x200,
    ROADSIDE_WARNING = 0x800,
    ROADSIDE_BARRICADE = 0x1000,
    ROADSIDE_S_CURVE = 0x2000,
    ROADSIDE_S_CURVE_2 = 0x4000
};

struct MirrorEdges {
    int buildingY[8];
    int rightX[8];
};

enum RoadSection {
    SECTION_LEFT_CURVE = 1,
    SECTION_RIGHT_CURVE = 2,
    SECTION_HILL = 3,
    SECTION_DIP = 4,
    SECTION_HILL_DIP = 5,
    SECTION_DIP_HILL = 6,
    SECTION_RIGHT_CLOSED = 7,
    SECTION_LEFT_CLOSED = 8,
    SECTION_NARROWS = 9,
    SECTION_S_CURVE_LR = 10,
    SECTION_S_CURVE_RL = 11
};

struct RoadView {
    int rightEdge;
    int roadIndex;
    int roadFraction;
    int distanceStep;
    int lateralX;
    int cameraY;
    int unused;
    int curveX;
    int roadHeight;
};

extern int g_yBuildingTopArray[15];

extern struct MirrorEdges g_mirGeom;

extern unsigned far *g_rightSideTerrainRow;
extern int g_mirYProj[8];

extern int g_roadLayout;
extern int g_onShoulder;
extern unsigned far *g_roadTerrain;
extern int g_roadLXArray[15];
extern int far *g_hereCurvePtr;
extern int far *g_heightRoadPtr;
extern int g_roadYProjectionArray[15];
extern int g_startDrawRows;
extern int g_rightXProjectionTable[15];
extern unsigned far *g_hereFlagsPtr;
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
