/**
 * @file ROAD.H
 * @brief Track generation, terrain flags and road projection.
 */

#ifndef ROAD_H
#define ROAD_H

#define ROAD_SURFACE_Y 2000

#include "CARPIC.H"

/**
 * @brief Per-side scenery word for one track sample.
 *
 * Each unsigned 16-bit word describes one side of a track sample.
 * @ref TERRAIN_TOWN selects the town aliases; otherwise the country scenery
 * meanings apply. Flags combine
 * independently. @ref ROADSIDE_UNUSED_15 is the top bit. Bits marked unused
 * have no generated feature or reader.
 */
enum RoadsideFlags {
    ROADSIDE_NONE = 0, /**< No roadside features. */
    /** Country post or town destination. */
    ROADSIDE_POST = 0x1,
    /** Town sign, cruise stop and turn. */
    ROADSIDE_DESTINATION = ROADSIDE_POST,
    /** Move gantry placement to the preceding sample; never generated. */
    ROADSIDE_GANTRY_PREV = 0x2,
    ROADSIDE_SMALL_ROCK = 0x4, /**< Country small rock. */
    /** Town raised building span. */
    ROADSIDE_BUILDING = ROADSIDE_SMALL_ROCK,
    ROADSIDE_LARGE_ROCK = 0x8,                    /**< Country large rock. */
    ROADSIDE_TALL_BUILDING = ROADSIDE_LARGE_ROCK, /**< Town full-height roof. */
    /** Town building wall-shading phase. */
    ROADSIDE_WALL_SHADE = 0x10,
    /** Tree at the town curb or country outer verge. */
    ROADSIDE_TREE = 0x20,
    /** Country second tree shape. */
    ROADSIDE_TREE_2 = 0x40,
    ROADSIDE_HYDRANT = ROADSIDE_TREE_2, /**< Town fire hydrant. */
    /** Country third tree shape. */
    ROADSIDE_TREE_3 = 0x80,
    /** Town sidewalk grass inset. */
    ROADSIDE_SIDEWALK_INSET = ROADSIDE_TREE_3,
    ROADSIDE_BUSH = 0x100, /**< Bush. */
    /** Country bush beyond the outer verge. */
    ROADSIDE_BUSH_OUTER = 0x200,
    ROADSIDE_UNUSED_10 = 0x400, /**< Unused bit. */
    /** Warning sign before a barricade. */
    ROADSIDE_WARNING = 0x800,
    /** Barricade; blocks shoulder passage. */
    ROADSIDE_BARRICADE = 0x1000,
    /** Right-side left-then-right curve sign. */
    ROADSIDE_S_CURVE = 0x2000,
    /** Right-side right-then-left curve sign. */
    ROADSIDE_S_CURVE_2 = 0x4000
};

/**
 * @brief Shared road word for one track sample.
 *
 * Unsigned 16-bit combinations of these flags and
 * @ref TERRAIN_STRAIGHT_GANTRY serve projection, scenery drawing and driving.
 * Gantry flags also request the elevated sign projection and obstruct both
 * shoulders. Gantry drawing chooses
 * @ref TERRAIN_CITY_GANTRY first, then @ref TERRAIN_SCENE_GANTRY, then
 * @ref TERRAIN_LEFT_GANTRY or @ref TERRAIN_STRAIGHT_GANTRY.
 */
enum TerrainFlags {
    TERRAIN_NONE = 0, /**< No shared road features. */
    /** Town sidewalks, buildings and roadside meanings. */
    TERRAIN_TOWN = 0x1,
    /** Right-side pole; obstructs the right shoulder. */
    TERRAIN_POLE = 0x2,
    TERRAIN_CROSS_STREET = 0x4, /**< Gap in town buildings and sidewalks. */
    TERRAIN_CENTER_LINE = 0x8,  /**< Draw a center-line dash. */
    /** White road sample before a race finish. */
    TERRAIN_FINISH_STRIPE = 0x10,
    TERRAIN_LEFT_CLOSED = 0x20,  /**< Move the left edge inward. */
    TERRAIN_RIGHT_CLOSED = 0x40, /**< Move the right edge inward. */
    /** Move both edges inward; overrides one-side flags. */
    TERRAIN_NARROW = 0x80,
    TERRAIN_SPEED_SIGN = 0x100,    /**< Speed-limit sign. */
    TERRAIN_CHEVRON_LEFT = 0x200,  /**< Left-chevron sign. */
    TERRAIN_CHEVRON_RIGHT = 0x400, /**< Right-chevron sign. */
    TERRAIN_NARROW_SIGN = 0x800,   /**< Road-narrows warning sign. */
    TERRAIN_CITY_GANTRY = 0x1000,  /**< Overhead city-limits scenery sign. */
    /** Overhead scene bitmap; race county line. */
    TERRAIN_SCENE_GANTRY = 0x2000,
    TERRAIN_LEFT_GANTRY = 0x4000 /**< Left arrow, or town Vegas billboard. */
};

/** Unused top bit of a @ref RoadsideFlags word. */
#define ROADSIDE_UNUSED_15 0x8000
/** Overhead straight arrow, or town Block Out ad; see @ref TerrainFlags. */
#define TERRAIN_STRAIGHT_GANTRY 0x8000

/** Features retained when clearing incidental scenery beneath a sign. */
#define ROADSIDE_SIGN_KEEP (ROADSIDE_POST | ROADSIDE_WARNING | ROADSIDE_BARRICADE)
/** Road constriction flags used by projection and rival lane selection. */
#define TERRAIN_WIDTH_MASK (TERRAIN_LEFT_CLOSED | TERRAIN_RIGHT_CLOSED | TERRAIN_NARROW)
/** All overhead signs and their shoulder obstructions. */
#define TERRAIN_GANTRY_MASK (TERRAIN_CITY_GANTRY | TERRAIN_SCENE_GANTRY | TERRAIN_LEFT_GANTRY | TERRAIN_STRAIGHT_GANTRY)


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

/** Right-side sample at the current seek position; @ref RoadsideFlags. */
extern unsigned far *g_hereRoadsideRightRowPtr;
extern int g_mirYProj[8];

extern int g_roadLayout;
extern int g_onShoulder;
/** Left-side sample at the current seek position; @ref RoadsideFlags. */
extern unsigned far *g_sampleLeftRoadside;
extern int g_roadLXArray[15];
/** Curve sample cursor, advanced or retreated by projection. */
extern int far *g_sampleCurveXReadPtr;
/** Road height cursor, advanced or retreated by projection. */
extern int far *g_elevationCursor;
extern int g_roadYProjectionArray[15];
extern int g_startDrawRows;
extern int g_rightXProjectionTable[15];
/** Shared sample cursor for projection and driving; @ref TerrainFlags. */
extern unsigned far *g_trackRoadWordsCursor;
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
