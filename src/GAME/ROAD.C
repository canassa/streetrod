#include <stdlib.h>
#include "ROAD.H"
#include "BITMAP.H"
#include "DASH.H"
#include "DRIVE.H"
#include "MAIN.H"
#include "PC.H"
#include "RACE.H"
#include "RANDOM.H"
#include "SCREEN.H"
#include "SCREEN2.H"
#include "SCREENS.H"
#include "SLOTS.H"
#include "SYS.H"
#include "VIEW.H"


static int g_buildX;
static int g_buildEntry;
static int g_bendStep;
static int near rnd(int n);
static void near build_left_curve(void);
static void near build_right_curve(void);
static void near build_s_curve_lr(void);
static void near build_s_curve_rl(void);
static void near build_hill(void);
static void near build_dip(void);
static void near build_hill_dip(void);
static void near build_dip_hill(void);
static void near build_right_closed(void);
static void near build_narrows(void);
static void near build_left_closed(void);

static int g_hillProfile[22] = {1, 2, 3, 4, 4, 4, 4, 3, 2, 1, 0, 0, -1, -2, -3, -4, -4, -4, -4, -3, -2, -1};
static int g_hillDipProfile[38] = {1,  2,  3,  4,  4,  4,  4,  3, 2, 1, 0, 0, -1, -2, -3, -4, -5, -5, -5,
                                   -5, -5, -5, -4, -3, -2, -1, 0, 0, 1, 2, 3, 4,  4,  4,  4,  3,  2,  1};
static int g_bumpProfile[17] = {1, 2, 3, 3, 3, 2, 1, 0, 0, 0, -1, -2, -3, -3, -3, -2, -1};
static int g_layoutLen = 12;
static enum RoadSection g_roadLayouts[10][12] = {
    {SECTION_RIGHT_CURVE, SECTION_HILL, SECTION_LEFT_CURVE, SECTION_RIGHT_CLOSED, SECTION_DIP, SECTION_S_CURVE_LR,
     SECTION_HILL_DIP, SECTION_LEFT_CLOSED, SECTION_DIP_HILL, SECTION_RIGHT_CURVE, SECTION_NARROWS, SECTION_LEFT_CURVE},
    {SECTION_S_CURVE_RL, SECTION_DIP, SECTION_LEFT_CLOSED, SECTION_LEFT_CURVE, SECTION_DIP_HILL, SECTION_NARROWS,
     SECTION_RIGHT_CURVE, SECTION_HILL_DIP, SECTION_RIGHT_CLOSED, SECTION_RIGHT_CURVE, SECTION_HILL,
     SECTION_LEFT_CURVE},
    {SECTION_LEFT_CURVE, SECTION_HILL_DIP, SECTION_LEFT_CURVE, SECTION_HILL, SECTION_LEFT_CLOSED, SECTION_S_CURVE_RL,
     SECTION_DIP_HILL, SECTION_RIGHT_CLOSED, SECTION_RIGHT_CURVE, SECTION_DIP, SECTION_NARROWS, SECTION_LEFT_CURVE},
    {SECTION_HILL, SECTION_LEFT_CURVE, SECTION_DIP, SECTION_NARROWS, SECTION_RIGHT_CURVE, SECTION_DIP_HILL,
     SECTION_LEFT_CLOSED, SECTION_LEFT_CURVE, SECTION_HILL, SECTION_LEFT_CLOSED, SECTION_S_CURVE_LR,
     SECTION_RIGHT_CURVE},
    {SECTION_S_CURVE_LR, SECTION_NARROWS, SECTION_HILL_DIP, SECTION_LEFT_CURVE, SECTION_DIP_HILL, SECTION_RIGHT_CURVE,
     SECTION_HILL, SECTION_RIGHT_CLOSED, SECTION_DIP, SECTION_RIGHT_CURVE, SECTION_LEFT_CLOSED, SECTION_RIGHT_CURVE},
    {SECTION_RIGHT_CURVE, SECTION_HILL, SECTION_LEFT_CLOSED, SECTION_S_CURVE_LR, SECTION_HILL_DIP, SECTION_S_CURVE_RL,
     SECTION_DIP, SECTION_RIGHT_CLOSED, SECTION_LEFT_CURVE, SECTION_DIP_HILL, SECTION_NARROWS, SECTION_RIGHT_CURVE},
    {SECTION_LEFT_CURVE, SECTION_HILL_DIP, SECTION_NARROWS, SECTION_LEFT_CURVE, SECTION_DIP, SECTION_RIGHT_CURVE,
     SECTION_HILL, SECTION_S_CURVE_LR, SECTION_LEFT_CLOSED, SECTION_DIP, SECTION_LEFT_CURVE, SECTION_NARROWS},
    {SECTION_RIGHT_CURVE, SECTION_DIP, SECTION_RIGHT_CLOSED, SECTION_S_CURVE_RL, SECTION_RIGHT_CURVE, SECTION_HILL_DIP,
     SECTION_LEFT_CLOSED, SECTION_DIP, SECTION_LEFT_CURVE, SECTION_RIGHT_CLOSED, SECTION_DIP_HILL, SECTION_RIGHT_CURVE},
    {SECTION_RIGHT_CURVE, SECTION_DIP_HILL, SECTION_LEFT_CLOSED, SECTION_S_CURVE_LR, SECTION_DIP_HILL,
     SECTION_LEFT_CURVE, SECTION_HILL, SECTION_RIGHT_CLOSED, SECTION_HILL_DIP, SECTION_LEFT_CURVE, SECTION_DIP,
     SECTION_NARROWS},
    {SECTION_LEFT_CURVE, SECTION_HILL, SECTION_LEFT_CURVE, SECTION_DIP_HILL, SECTION_LEFT_CURVE, SECTION_DIP,
     SECTION_NARROWS, SECTION_S_CURVE_LR, SECTION_DIP_HILL, SECTION_LEFT_CLOSED, SECTION_RIGHT_CURVE, SECTION_HILL}};
static struct Bitmap g_egaPage1 = {
    SCREEN_WIDTH,     SCREEN_HEIGHT,
    EGA_PLANE_BYTES,  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0},
    {EGA_PAGE1_BITS}, {0},
    EGA_PITCH,        VID_EGA};
static struct Bitmap g_egaPage2 = {
    SCREEN_WIDTH,     SCREEN_HEIGHT,
    EGA_PLANE_BYTES,  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0},
    {EGA_PAGE2_BITS}, {0},
    EGA_PITCH,        VID_EGA};
int g_roadLayout = 0;
int g_onShoulder = 0;
static int g_turnRight = 0;
static int g_turnLeft = 0;

/* Near wrapper around rn_int, used by the track builder */

static int near rnd(int n)
{
    return rn_int(n);
}


static void near build_left_curve(void)
{
    int i;

    for (i = g_buildEntry; i < g_buildEntry + 50; i++)
        g_terrain2[i] = g_buildX;
    g_terrain3[g_buildEntry] |= TERRAIN_CHEVRON_LEFT;
    g_buildEntry += 5;
    g_terrain0[g_buildEntry] &= ROADSIDE_SIGN_KEEP;
    g_terrain1[g_buildEntry] &= ROADSIDE_SIGN_KEEP;
    g_terrain3[g_buildEntry] |= TERRAIN_CHEVRON_LEFT;
    g_buildEntry += 6;
    for (i = g_buildX / g_bendStep; i < g_buildX / g_bendStep + 40; i++, g_buildEntry++) {
        if (g_buildEntry < 0x442) {
            g_terrain3[g_buildEntry] |= TERRAIN_CENTER_LINE;
            g_terrain2[g_buildEntry] = i * g_bendStep;
        } else
            break;
    }
    g_buildX = i * g_bendStep;
}


static void near build_right_curve(void)
{
    int i;

    for (i = g_buildEntry; i < g_buildEntry + 50; i++)
        g_terrain2[i] = g_buildX;
    g_terrain0[g_buildEntry] &= ROADSIDE_SIGN_KEEP;
    g_terrain1[g_buildEntry] &= ROADSIDE_SIGN_KEEP;
    g_terrain3[g_buildEntry] |= TERRAIN_CHEVRON_RIGHT;
    g_buildEntry += 3;
    g_terrain3[g_buildEntry] |= TERRAIN_CHEVRON_RIGHT;
    g_buildEntry += 3;
    for (i = g_buildX / g_bendStep; i > g_buildX / g_bendStep - 45; i--, g_buildEntry++) {
        if (g_buildEntry < 0x442) {
            g_terrain3[g_buildEntry] |= TERRAIN_CENTER_LINE;
            g_terrain2[g_buildEntry] = i * g_bendStep;
        } else
            break;
    }
    g_buildX = i * g_bendStep;
}


static void near build_s_curve_lr(void)
{
    int i;

    for (i = g_buildEntry; i < g_buildEntry + 90; i++)
        g_terrain2[i] = g_buildX;
    g_buildEntry++;
    g_terrain0[g_buildEntry] &= ROADSIDE_SIGN_KEEP;
    g_terrain1[g_buildEntry] &= ROADSIDE_SIGN_KEEP;
    g_terrain1[g_buildEntry] |= ROADSIDE_S_CURVE;
    g_buildEntry += 6;
    g_terrain1[g_buildEntry] |= ROADSIDE_S_CURVE;
    for (i = g_buildX / g_bendStep; i < g_buildX / g_bendStep + 40; i++, g_buildEntry++) {
        if (g_buildEntry < 0x442) {
            g_terrain3[g_buildEntry] |= TERRAIN_CENTER_LINE;
            g_terrain2[g_buildEntry] = i * g_bendStep;
        } else
            break;
    }
    g_buildX = i * g_bendStep;
    for (; i > g_buildX / g_bendStep - 43; i--, g_buildEntry++) {
        if (g_buildEntry < 0x442) {
            g_terrain3[g_buildEntry] |= TERRAIN_CENTER_LINE;
            g_terrain2[g_buildEntry] = i * g_bendStep;
        } else
            break;
    }
    g_buildX = i * g_bendStep;
}


static void near build_s_curve_rl(void)
{
    int i;

    for (i = g_buildEntry; i < g_buildEntry + 90; i++)
        g_terrain2[i] = g_buildX;
    g_buildEntry++;
    g_terrain0[g_buildEntry] &= ROADSIDE_SIGN_KEEP;
    g_terrain1[g_buildEntry] &= ROADSIDE_SIGN_KEEP;
    g_terrain1[g_buildEntry] |= ROADSIDE_S_CURVE_2;
    g_buildEntry += 6;
    g_terrain1[g_buildEntry] |= ROADSIDE_S_CURVE_2;
    for (i = g_buildX / g_bendStep; i > g_buildX / g_bendStep - 35; i--, g_buildEntry++) {
        if (g_buildEntry < 0x442) {
            g_terrain3[g_buildEntry] |= TERRAIN_CENTER_LINE;
            g_terrain2[g_buildEntry] = i * g_bendStep;
        } else
            break;
    }
    g_buildX = i * g_bendStep;
    for (; i < g_buildX / g_bendStep + 48; i++, g_buildEntry++) {
        if (g_buildEntry < 0x442) {
            g_terrain3[g_buildEntry] |= TERRAIN_CENTER_LINE;
            g_terrain2[g_buildEntry] = i * g_bendStep;
        } else
            break;
    }
    g_buildX = i * g_bendStep;
}


static void near build_hill(void)
{
    int i, start;

    start = g_buildEntry;
    g_buildEntry += 20;
    for (i = 0; i < 22; i++) {
        if (g_buildEntry < 0x442)
            g_terrain4[g_buildEntry++] = g_hillProfile[i];
        else
            break;
    }
    g_buildEntry += 8;
    for (i = start; i < g_buildEntry; i++)
        g_terrain2[i] = g_buildX;
}

/* Builds a terrain hill section */

static void near build_dip(void)
{
    int i, start;

    start = g_buildEntry;
    g_buildEntry += 10;
    for (i = 0; i < 22; i++) {
        if (g_buildEntry < 0x442)
            g_terrain4[g_buildEntry++] = -g_hillProfile[i];
        else
            break;
    }
    g_buildEntry += 18;
    for (i = start; i < g_buildEntry; i++)
        g_terrain2[i] = g_buildX;
}

/* Builds a terrain hill section */

static void near build_hill_dip(void)
{
    int i, start;

    start = g_buildEntry;
    g_buildEntry += 6;
    for (i = 0; i < 38; i++) {
        if (g_buildEntry < 0x442)
            g_terrain4[g_buildEntry++] = g_hillDipProfile[i];
        else
            break;
    }
    g_buildEntry += 6;
    for (i = start; i < g_buildEntry; i++)
        g_terrain2[i] = g_buildX;
}

/* Builds a terrain hill section */

static void near build_dip_hill(void)
{
    int i, start;

    start = g_buildEntry;
    g_buildEntry += 8;
    for (i = 0; i < 38; i++) {
        if (g_buildEntry < 0x442)
            g_terrain4[g_buildEntry++] = -g_hillDipProfile[i];
        else
            break;
    }
    g_buildEntry += 4;
    for (i = start; i < g_buildEntry; i++)
        g_terrain2[i] = g_buildX;
}


static void near build_right_closed(void)
{
    int i, start;

    start = g_buildEntry;
    g_buildEntry += 3;
    g_terrain1[g_buildEntry] |= ROADSIDE_WARNING;
    g_buildEntry += 4;
    g_terrain3[g_buildEntry++] &= ~(TERRAIN_GANTRY_MASK | TERRAIN_CENTER_LINE);
    g_terrain1[g_buildEntry++] |= ROADSIDE_BARRICADE;
    for (i = 0; i < 10; i++) {
        g_terrain3[g_buildEntry] |= TERRAIN_RIGHT_CLOSED;
        g_terrain3[g_buildEntry++] &= ~(TERRAIN_GANTRY_MASK | TERRAIN_CENTER_LINE);
    }
    g_terrain3[g_buildEntry++] &= ~(TERRAIN_GANTRY_MASK | TERRAIN_CENTER_LINE);
    g_buildEntry += 3;
    for (i = start; i < g_buildEntry; i++)
        g_terrain2[i] = g_buildX;
}


static void near build_narrows(void)
{
    int i, start;

    start = g_buildEntry;
    g_terrain3[g_buildEntry] |= TERRAIN_NARROW_SIGN;
    g_buildEntry += 4;
    g_terrain0[g_buildEntry] &= ROADSIDE_SIGN_KEEP;
    g_terrain1[g_buildEntry] &= ROADSIDE_SIGN_KEEP;
    g_terrain3[g_buildEntry] |= TERRAIN_NARROW_SIGN;
    g_buildEntry += 6;
    for (i = 0; i < 20; i++) {
        g_terrain3[g_buildEntry] |= TERRAIN_NARROW;
        g_terrain3[g_buildEntry++] &= ~(TERRAIN_GANTRY_MASK | TERRAIN_CENTER_LINE);
    }
    g_terrain3[g_buildEntry] &= ~TERRAIN_GANTRY_MASK;
    for (i = start; i < g_buildEntry; i++)
        g_terrain2[i] = g_buildX;
}


static void near build_left_closed(void)
{
    int i, start;

    start = g_buildEntry;
    g_buildEntry += 4;
    g_terrain0[g_buildEntry] |= ROADSIDE_WARNING;
    g_buildEntry += 4;
    g_terrain3[g_buildEntry++] &= ~(TERRAIN_GANTRY_MASK | TERRAIN_CENTER_LINE);
    g_terrain0[g_buildEntry++] |= ROADSIDE_BARRICADE;
    for (i = 0; i < 10; i++) {
        g_terrain3[g_buildEntry] |= TERRAIN_LEFT_CLOSED;
        g_terrain3[g_buildEntry++] &= ~(TERRAIN_GANTRY_MASK | TERRAIN_CENTER_LINE);
    }
    g_terrain3[g_buildEntry++] &= ~(TERRAIN_GANTRY_MASK | TERRAIN_CENTER_LINE);
    g_buildEntry += 2;
    for (i = start; i < g_buildEntry; i++)
        g_terrain2[i] = g_buildX;
}

/* Builds the track profile arrays from the section list of the chosen layout. */

void rd_build_layout(int n)
{
    int i;

    g_buildEntry = 0x186;
    g_buildX = 0;
    g_bendStep = g_hasSlowCpu ? 2 : 3;
    for (i = DRAG_FINISH_INDEX; i < 0x460; i++) {
        g_terrain2[i] = 0;
        g_terrain4[i] = 0;
        g_terrain3[i] &= ~(TERRAIN_CENTER_LINE | TERRAIN_WIDTH_MASK | TERRAIN_CHEVRON_LEFT | TERRAIN_CHEVRON_RIGHT |
                           TERRAIN_NARROW_SIGN);
        g_terrain1[i] &= ~(ROADSIDE_WARNING | ROADSIDE_BARRICADE | ROADSIDE_S_CURVE | ROADSIDE_S_CURVE_2);
        g_terrain0[i] &= ~(ROADSIDE_WARNING | ROADSIDE_BARRICADE);
        if ((i + 1) % (g_hasSlowCpu * 4 + 4) == 0)
            g_terrain3[i] |= TERRAIN_CENTER_LINE;
    }
    if (n == 100)
        g_roadLayout = rnd(10);
    for (i = 0; i < g_layoutLen; i++) {
        switch (g_roadLayouts[g_roadLayout][i]) {
        case SECTION_LEFT_CURVE:
            build_left_curve();
            break;
        case SECTION_RIGHT_CURVE:
            build_right_curve();
            break;
        case SECTION_HILL:
            build_hill();
            break;
        case SECTION_DIP:
            build_dip();
            break;
        case SECTION_HILL_DIP:
            build_hill_dip();
            break;
        case SECTION_DIP_HILL:
            build_dip_hill();
            break;
        case SECTION_RIGHT_CLOSED:
            build_right_closed();
            break;
        case SECTION_LEFT_CLOSED:
            build_left_closed();
            break;
        case SECTION_NARROWS:
            build_narrows();
            break;
        case SECTION_S_CURVE_LR:
            build_s_curve_lr();
            break;
        case SECTION_S_CURVE_RL:
            build_s_curve_rl();
            break;
        }
    }
    for (; g_buildEntry < 0x460; g_buildEntry++) {
        g_terrain2[g_buildEntry] = g_buildX;
        g_terrain4[g_buildEntry] = 0;
    }
}

/* Builds the terrain flag arrays for a new track: start/finish area, hills,
   then the random features, and finally calls the track builder. */

void rd_generate(void)
{
    int cnt, i, j;

    if (r_vid.mode == VID_EGA) {
        g_driveDisplayPage = &g_egaPage0;
        g_driveScreenB = &g_egaPage1;
        g_bufferSceneA = &g_egaPage2;
    } else {
        g_driveDisplayPage = r_vid.front;
        g_driveScreenB = r_vid.back;
        g_bufferSceneA = &g_screens[2];
    }
    cnt = g_hasSlowCpu;
    for (i = 0; i < 0xb4; i++) {
        g_terrain4[i] = 0;
        g_terrain2[i] = 0;
        g_terrain3[i] = TERRAIN_NONE;
        g_terrain1[i] = ROADSIDE_NONE;
        g_terrain0[i] = ROADSIDE_NONE;
        g_terrain3[i] |= TERRAIN_TOWN;
        if ((i + 1) % (cnt * 4 + 4) == 0)
            g_terrain3[i] |= TERRAIN_CENTER_LINE;
        if ((i + 2) % 16 == 0) {
            g_terrain3[i + 2] |= TERRAIN_CROSS_STREET;
            g_terrain3[i + 1] |= TERRAIN_CROSS_STREET;
            g_terrain3[i] |= TERRAIN_CROSS_STREET;
            g_terrain1[i - 1] |= ROADSIDE_HYDRANT;
            g_terrain0[i + 3] |= ROADSIDE_HYDRANT;
            g_terrain3[i + 1] |= TERRAIN_TOWN;
            g_terrain3[i + 2] |= TERRAIN_TOWN;
            g_terrain3[i + 3] |= TERRAIN_TOWN;
            g_terrain4[i + 1] = 0;
            g_terrain2[i + 1] = 0;
            g_terrain4[i + 2] = 0;
            g_terrain2[i + 2] = 0;
            g_terrain4[i + 3] = 0;
            g_terrain2[i + 3] = 0;
            i += 3;
        } else {
            if ((i + 4) % 16 == 0 || (i + 8) % 16 == 0 || (i + 12) % 16 == 0) {
                if (rnd(2) == 0) {
                    g_terrain0[i - 1] |= ROADSIDE_BUILDING;
                    g_terrain0[i] |= ROADSIDE_BUILDING;
                    if (rnd(3) == 0) {
                        g_terrain0[i - 1] |= ROADSIDE_TALL_BUILDING;
                        g_terrain0[i] |= ROADSIDE_TALL_BUILDING;
                    }
                    g_terrain0[i] |= ROADSIDE_WALL_SHADE;
                } else {
                    g_terrain0[i - 2] |= ROADSIDE_BUILDING;
                    g_terrain0[i] |= ROADSIDE_BUILDING;
                    g_terrain0[i - 1] |= ROADSIDE_BUILDING;
                    if (rnd(3) == 0) {
                        g_terrain0[i - 2] |= ROADSIDE_TALL_BUILDING;
                        g_terrain0[i] |= ROADSIDE_TALL_BUILDING;
                        g_terrain0[i - 1] |= ROADSIDE_TALL_BUILDING;
                    }
                    g_terrain0[i - 1] |= ROADSIDE_WALL_SHADE;
                }
            } else
                g_terrain0[i] |= ROADSIDE_SIDEWALK_INSET;
            if ((i + 5) % 16 == 0 || (i + 9) % 16 == 0 || (i + 12) % 16 == 0) {
                if (rnd(3) != 0) {
                    g_terrain1[i - 1] |= ROADSIDE_BUILDING;
                    g_terrain1[i] |= ROADSIDE_BUILDING;
                    if (rnd(2) == 0) {
                        g_terrain1[i - 1] |= ROADSIDE_TALL_BUILDING;
                        g_terrain1[i] |= ROADSIDE_TALL_BUILDING;
                    }
                    g_terrain1[i] |= ROADSIDE_WALL_SHADE;
                } else {
                    g_terrain1[i - 2] |= ROADSIDE_BUILDING;
                    g_terrain1[i] |= ROADSIDE_BUILDING;
                    g_terrain1[i - 1] |= ROADSIDE_BUILDING;
                    if (rnd(4) == 0) {
                        g_terrain1[i - 2] |= ROADSIDE_TALL_BUILDING;
                        g_terrain1[i] |= ROADSIDE_TALL_BUILDING;
                        g_terrain1[i - 1] |= ROADSIDE_TALL_BUILDING;
                    }
                    g_terrain1[i - 1] |= ROADSIDE_WALL_SHADE;
                }
            } else
                g_terrain1[i] |= ROADSIDE_SIDEWALK_INSET;
            if (rnd(cnt * 40 + 6) == 0 && !(g_terrain0[i] & ROADSIDE_WALL_SHADE))
                g_terrain0[i] |= ROADSIDE_TREE;
            if (rnd(cnt * 40 + 7) == 0 && !(g_terrain1[i] & ROADSIDE_WALL_SHADE))
                g_terrain1[i] |= ROADSIDE_TREE;
            if (rnd(cnt * 40 + 3) == 0 && !(g_terrain0[i] & ROADSIDE_WALL_SHADE))
                g_terrain0[i] |= ROADSIDE_BUSH;
            if (rnd(cnt * 40 + 4) == 0 && !(g_terrain1[i] & ROADSIDE_WALL_SHADE))
                g_terrain1[i] |= ROADSIDE_BUSH;
        }
    }
    g_terrain3[rnd(45) + 10] |= TERRAIN_LEFT_GANTRY;
    g_terrain3[rnd(45) + 10] |= TERRAIN_STRAIGHT_GANTRY;
    g_terrain3[rnd(55) + 65] |= TERRAIN_LEFT_GANTRY;
    g_terrain3[rnd(55) + 65] |= TERRAIN_STRAIGHT_GANTRY;
    g_terrain3[rnd(55) + 115] |= TERRAIN_LEFT_GANTRY;
    g_terrain3[rnd(55) + 115] |= TERRAIN_STRAIGHT_GANTRY;
    g_terrain1[44] |= ROADSIDE_DESTINATION;
    g_terrain0[29] |= ROADSIDE_BARRICADE;
    g_terrain0[24] |= ROADSIDE_WARNING;
    g_terrain0[108] |= ROADSIDE_DESTINATION;
    g_terrain0[87] |= ROADSIDE_BARRICADE;
    g_terrain0[82] |= ROADSIDE_WARNING;
    g_terrain0[156] |= ROADSIDE_DESTINATION;
    g_terrain0[135] |= ROADSIDE_BARRICADE;
    g_terrain0[130] |= ROADSIDE_WARNING;
    for (i = 0xb4; i < 0x460; i++) {
        g_terrain4[i] = 0;
        g_terrain2[i] = 0;
        g_terrain3[i] = TERRAIN_NONE;
        g_terrain0[i] = ROADSIDE_NONE;
        g_terrain1[i] = ROADSIDE_NONE;
        if ((i + 1) % (g_hasSlowCpu * 4 + 4) == 0)
            g_terrain3[i] |= TERRAIN_CENTER_LINE;
        if (i % (cnt * 4 + 4) == 0) {
            g_terrain0[i] |= ROADSIDE_POST;
            g_terrain1[i] |= ROADSIDE_POST;
        }
        if ((i + 2) % (cnt * 8 + 8) == 0)
            g_terrain3[i] |= TERRAIN_POLE;
        if (rnd(cnt * 200 + 7) == 0)
            g_terrain0[i] |= ROADSIDE_TREE;
        if (rnd(cnt * 200 + 9) == 0)
            g_terrain1[i] |= ROADSIDE_TREE;
        if (cnt == 0) {
            if (rnd(35) == 0) {
                g_terrain0[i] |= ROADSIDE_TREE_2;
                g_terrain0[i - 1] |= ROADSIDE_TREE_2;
                g_terrain0[i - 2] |= ROADSIDE_TREE_2;
                g_terrain0[i] &= ~ROADSIDE_TREE;
                g_terrain0[i - 1] &= ~ROADSIDE_TREE;
                g_terrain0[i - 2] &= ~ROADSIDE_TREE;
            }
            if (rnd(25) == 0) {
                g_terrain1[i] |= ROADSIDE_TREE_2;
                g_terrain1[i - 2] |= ROADSIDE_TREE_2;
                g_terrain1[i] &= ~ROADSIDE_TREE;
                g_terrain1[i - 2] &= ~ROADSIDE_TREE;
            }
        } else {
            if (rnd(cnt * 100 + 35) == 0) {
                g_terrain0[i] |= ROADSIDE_TREE_2;
                g_terrain0[i] &= ~ROADSIDE_TREE;
            }
            if (rnd(cnt * 100 + 25) == 0) {
                g_terrain1[i] |= ROADSIDE_TREE_2;
                g_terrain1[i] &= ~ROADSIDE_TREE;
            }
        }
        if (rnd(cnt * 100 + 7) == 0 && !(g_terrain0[i] & ROADSIDE_TREE_3))
            g_terrain0[i] |= ROADSIDE_TREE_3;
        if (rnd(cnt * 100 + 9) == 0 && !(g_terrain1[i] & ROADSIDE_TREE_3))
            g_terrain1[i] |= ROADSIDE_TREE_3;
        if (rnd(cnt * 100 + 21) == 0)
            g_terrain0[i] |= ROADSIDE_SMALL_ROCK;
        if (rnd(cnt * 100 + 25) == 0)
            g_terrain1[i] |= ROADSIDE_SMALL_ROCK;
        if (rnd(cnt * 100 + 23) == 0 && !(g_terrain0[i] & ROADSIDE_SMALL_ROCK))
            g_terrain0[i] |= ROADSIDE_LARGE_ROCK;
        if (rnd(cnt * 100 + 30) == 0 && !(g_terrain1[i] & ROADSIDE_SMALL_ROCK))
            g_terrain1[i] |= ROADSIDE_LARGE_ROCK;
        if (rnd(cnt * 50 + 4) == 0) {
            g_terrain0[i] |= ROADSIDE_BUSH;
            if (rnd(2) == 0)
                g_terrain0[i] |= ROADSIDE_BUSH_OUTER;
        }
        if (rnd(cnt * 50 + 4) == 0) {
            g_terrain1[i] |= ROADSIDE_BUSH;
            if (rnd(2) == 0)
                g_terrain1[i] |= ROADSIDE_BUSH_OUTER;
        }
    }
    g_terrain0[379] &= ROADSIDE_SIGN_KEEP;
    g_terrain1[379] &= ROADSIDE_SIGN_KEEP;
    g_terrain3[379] |= (TERRAIN_CITY_GANTRY | TERRAIN_FINISH_STRIPE);
    g_terrain0[1019] &= ROADSIDE_SIGN_KEEP;
    g_terrain1[1019] &= ROADSIDE_SIGN_KEEP;
    g_terrain3[1019] |= (TERRAIN_SCENE_GANTRY | TERRAIN_FINISH_STRIPE);
    {
        i = rnd(140) + 220;
        j = (g_terrain1[i] & ROADSIDE_GANTRY_PREV) ? i - 1 : i;
        g_terrain0[j] &= ROADSIDE_SIGN_KEEP;
        g_terrain1[j] &= ROADSIDE_SIGN_KEEP;
        g_terrain3[j] |= TERRAIN_STRAIGHT_GANTRY;
        i = rnd(600) + 390;
        j = (g_terrain1[i] & ROADSIDE_GANTRY_PREV) ? i - 1 : i;
        g_terrain0[j] &= ROADSIDE_SIGN_KEEP;
        g_terrain1[j] &= ROADSIDE_SIGN_KEEP;
        g_terrain3[j] |= TERRAIN_STRAIGHT_GANTRY;
    }
    i = rnd(600) + 390;
    j = (g_terrain1[i] & ROADSIDE_GANTRY_PREV) ? i - 1 : i;
    g_terrain0[j] &= ROADSIDE_SIGN_KEEP;
    g_terrain1[j] &= ROADSIDE_SIGN_KEEP;
    g_terrain3[j] |= TERRAIN_LEFT_GANTRY;
    i = 220;
    do {
        for (j = 0; j < 17; j++)
            g_terrain4[i++] = g_bumpProfile[j];
        i += 20;
        for (j = 0; j < 17; j++)
            g_terrain4[i++] = -g_bumpProfile[j];
        i += 30;
    } while (i + 90 < DRAG_FINISH_INDEX);
    g_terrain3[385] |= TERRAIN_SPEED_SIGN;
    rd_build_layout(100);
}

unsigned far *g_roadTerrain;
int g_roadLXArray[15];
int far *g_hereCurvePtr;
int far *g_heightRoadPtr;
int g_roadYProjectionArray[15];
int g_startDrawRows;
int g_yBuildingTopArray[15];
int g_rightXProjectionTable[15];
unsigned far *g_hereFlagsPtr;
int g_lhsRdMirrorX[8];
int g_roadOuterLeftXArray[15];
int g_rhsOuterXArray[15];
unsigned far *g_rightSideTerrainRow;
int g_mirYProj[8];
struct MirrorEdges g_mirGeom;

/* Points the terrain pointers at entry n of each terrain array. */

void rd_seek(int n)
{
    g_roadTerrain = &g_terrain0[n];
    g_rightSideTerrainRow = &g_terrain1[n];
    g_hereFlagsPtr = &g_terrain3[n];
    g_heightRoadPtr = &g_terrain4[n];
    g_hereCurvePtr = &g_terrain2[n];
}


int rd_proj_x(int a, int b, int c)
{
    int u, v;

    b += a * c >> 2;
    v = g_trackViewMotion.lateralX - b;
    u = v << 2;
    if (a == 0)
        a++;
    u = u / a;
    return 0xa0 - u;
}


int rd_proj_y(int a, int b, int c)
{
    int u, v;

    b += a * c;
    v = b - g_trackViewMotion.cameraY;
    u = v << 2;
    if (a == 0)
        a++;
    u = u / a;
    return u + 0x3c;
}


int rd_mirror_x(int a, int b, int c)
{
    int u, v;

    b += a * c >> 2;
    v = g_trackViewMotion.lateralX - b;
    if (a == 0)
        a++;
    u = v / a;
    return 0x118 - u;
}


int rd_mirror_y(int a, int b, int c)
{
    int u, v;

    b += a * c;
    v = b - g_trackViewMotion.cameraY;
    if (a == 0)
        a++;
    u = v / a;
    return u + 0x24;
}

/* Resets the terrain scroll state for a new track. */

void rd_reset(void)
{
    if (g_runRacingFlag)
        g_trackViewMotion.roadIndex = 200;
    else
        switch (g_scene) {
        case SCENE_GARAGE:
            g_trackViewMotion.roadIndex = 10;
            break;
        case SCENE_BOBS:
            g_trackViewMotion.roadIndex = 65;
            break;
        case SCENE_GAS_STATION:
            g_trackViewMotion.roadIndex = 115;
            break;
        }
    rd_seek(g_trackViewMotion.roadIndex);
    g_leftEdgeX = 1500;
    g_edgeXR = 3300;
    g_trackViewMotion.roadFraction = 0;
    g_trackViewMotion.lateralX = 2560;
    g_trackViewMotion.cameraY = 1900;
    g_trackViewMotion.unused = 0;
    g_trackViewMotion.roadHeight = 0;
    g_trackViewMotion.curveX = 0;
    g_trackViewMotion.distanceStep = 0;
    g_rivalLeft = 1;
    g_swapArmedRivalValue = !rn_int(2);
    g_ramAllowed = 1;
}

/* Fills the per-row projection tables (15 rows, the first one peeled) from the
   terrain pointers, walking the track forward. rd_project_behind is the backward twin. */

void rd_project_ahead(void)
{
    int *p5l, k7;
    enum TerrainFlags widthBits;
    int *p11, rowz;
    BOOL needTops;
    int *p2, *p3, d1, d2, *p42, *p6;

    needTops = (*g_hereFlagsPtr & TERRAIN_TOWN) || (*g_hereFlagsPtr & TERRAIN_GANTRY_MASK);
    widthBits = *g_hereFlagsPtr++ & TERRAIN_WIDTH_MASK;
    p11 = g_rightXProjectionTable;
    p2 = g_roadLXArray;
    p3 = g_roadYProjectionArray;
    p42 = g_roadOuterLeftXArray;
    p5l = g_rhsOuterXArray;
    p6 = g_yBuildingTopArray;
    rowz = 4;
    d1 = g_trackViewMotion.roadHeight - *g_heightRoadPtr++;
    d2 = g_trackViewMotion.curveX - *g_hereCurvePtr++;
    *p3++ = rd_proj_y(rowz, ROAD_SURFACE_Y, d1);
    if (widthBits) {
        if (widthBits & TERRAIN_NARROW) {
            *p11++ = rd_proj_x(rowz, 0xaf0, d2);
            *p2++ = rd_proj_x(rowz, 2000, d2);
        } else if (widthBits & TERRAIN_LEFT_CLOSED) {
            *p11++ = rd_proj_x(rowz, 0xce4, d2);
            *p2++ = rd_proj_x(rowz, 2000, d2);
        } else {
            *p11++ = rd_proj_x(rowz, 0xaf0, d2);
            *p2++ = rd_proj_x(rowz, 1500, d2);
        }
    } else {
        *p11++ = rd_proj_x(rowz, 0xce4, d2);
        *p2++ = rd_proj_x(rowz, 1500, d2);
    }
    *p5l++ = rd_proj_x(rowz, 0xe74, d2);
    *p42++ = rd_proj_x(rowz, 0x44c, d2);
    if (needTops)
        *p6++ = rd_proj_y(rowz, 0x76c, d1);
    else
        p6++;
    rowz = 12 - g_trackViewMotion.roadFraction;
    k7 = 14;
    do {
        needTops = (*g_hereFlagsPtr & TERRAIN_TOWN) || (*g_hereFlagsPtr & TERRAIN_GANTRY_MASK);
        widthBits = *g_hereFlagsPtr & TERRAIN_WIDTH_MASK;
        g_hereFlagsPtr++;
        d1 = g_trackViewMotion.roadHeight - *g_heightRoadPtr;
        g_heightRoadPtr++;
        d2 = g_trackViewMotion.curveX - *g_hereCurvePtr;
        g_hereCurvePtr++;
        if (widthBits) {
            if (widthBits & TERRAIN_NARROW) {
                *p11 = rd_proj_x(rowz, 0xaf0, d2);
                p11++;
                *p2 = rd_proj_x(rowz, 2000, d2);
                p2++;
            } else if (widthBits & TERRAIN_LEFT_CLOSED) {
                *p11 = rd_proj_x(rowz, 0xce4, d2);
                p11++;
                *p2 = rd_proj_x(rowz, 2000, d2);
                p2++;
            } else {
                *p11 = rd_proj_x(rowz, 0xaf0, d2);
                p11++;
                *p2 = rd_proj_x(rowz, 1500, d2);
                p2++;
            }
        } else {
            *p11 = rd_proj_x(rowz, 0xce4, d2);
            p11++;
            *p2 = rd_proj_x(rowz, 1500, d2);
            p2++;
        }
        *p3 = rd_proj_y(rowz, ROAD_SURFACE_Y, d1);
        p3++;
        *p5l = rd_proj_x(rowz, 0xe74, d2);
        p5l++;
        *p42 = rd_proj_x(rowz, 0x44c, d2);
        p42++;
        if (needTops)
            *p6 = rd_proj_y(rowz, 0x76c, d1);
        p6++;
        rowz += 8;
    } while (--k7);
}

/* Fills the per-row projection tables (8 rows, the first one peeled) from the
   terrain pointers, walking the track backward. rd_project_ahead is the forward twin. */

void rd_project_behind(void)
{
    int *p5l, k7;
    enum TerrainFlags widthBits;
    int *p11, rowz;
    BOOL needTops;
    int *p2, *p3, d1, d2, *p42, *p6;

    needTops = (*g_hereFlagsPtr & TERRAIN_TOWN) || (*g_hereFlagsPtr & TERRAIN_GANTRY_MASK);
    widthBits = *g_hereFlagsPtr-- & TERRAIN_WIDTH_MASK;
    p11 = g_mirGeom.rightX;
    p2 = g_lhsRdMirrorX;
    p3 = g_mirYProj;
    p42 = g_edgesRoad.leftOuterX;
    p5l = g_edgesRoad.rightOuterX;
    p6 = g_mirGeom.buildingY;
    rowz = 8;
    d1 = *g_heightRoadPtr-- - g_trackViewMotion.roadHeight;
    d2 = *g_hereCurvePtr-- - g_trackViewMotion.curveX;
    *p3++ = rd_mirror_y(rowz, ROAD_SURFACE_Y, d1);
    if (widthBits) {
        if (widthBits & TERRAIN_NARROW) {
            *p11++ = rd_mirror_x(rowz, 0xaf0, d2);
            *p2++ = rd_mirror_x(rowz, 2000, d2);
        } else if (widthBits & TERRAIN_LEFT_CLOSED) {
            *p11++ = rd_mirror_x(rowz, 0xce4, d2);
            *p2++ = rd_mirror_x(rowz, 2000, d2);
        } else {
            *p11++ = rd_mirror_x(rowz, 0xaf0, d2);
            *p2++ = rd_mirror_x(rowz, 1500, d2);
        }
    } else {
        *p11++ = rd_mirror_x(rowz, 0xce4, d2);
        *p2++ = rd_mirror_x(rowz, 1500, d2);
    }
    *p5l++ = rd_mirror_x(rowz, 0xe74, d2);
    *p42++ = rd_mirror_x(rowz, 0x44c, d2);
    if (needTops)
        *p6++ = rd_mirror_y(rowz, 0x76c, d1);
    else
        p6++;
    rowz = g_trackViewMotion.roadFraction + 8;
    k7 = 7;
    do {
        needTops = (*g_hereFlagsPtr & TERRAIN_TOWN) || (*g_hereFlagsPtr & TERRAIN_GANTRY_MASK);
        widthBits = *g_hereFlagsPtr & TERRAIN_WIDTH_MASK;
        g_hereFlagsPtr--;
        d1 = *g_heightRoadPtr - g_trackViewMotion.roadHeight;
        g_heightRoadPtr--;
        d2 = *g_hereCurvePtr - g_trackViewMotion.curveX;
        g_hereCurvePtr--;
        if (widthBits) {
            if (widthBits & TERRAIN_NARROW) {
                *p11 = rd_mirror_x(rowz, 0xaf0, d2);
                p11++;
                *p2 = rd_mirror_x(rowz, 2000, d2);
                p2++;
            } else if (widthBits & TERRAIN_LEFT_CLOSED) {
                *p11 = rd_mirror_x(rowz, 0xce4, d2);
                p11++;
                *p2 = rd_mirror_x(rowz, 2000, d2);
                p2++;
            } else {
                *p11 = rd_mirror_x(rowz, 0xaf0, d2);
                p11++;
                *p2 = rd_mirror_x(rowz, 1500, d2);
                p2++;
            }
        } else {
            *p11 = rd_mirror_x(rowz, 0xce4, d2);
            p11++;
            *p2 = rd_mirror_x(rowz, 1500, d2);
            p2++;
        }
        *p3 = rd_mirror_y(rowz, ROAD_SURFACE_Y, d1);
        p3++;
        *p5l = rd_mirror_x(rowz, 0xe74, d2);
        p5l++;
        *p42 = rd_mirror_x(rowz, 0x44c, d2);
        p42++;
        if (needTops)
            *p6 = rd_mirror_y(rowz, 0x76c, d1);
        p6++;
        rowz += 8;
    } while (--k7);
}

/* Rebuilds the projection tables for both directions, then waits 9 ticks. */

void rd_draw_frame(void)
{
    long deadline;

    rd_seek(g_trackViewMotion.roadIndex);
    rd_project_ahead();
    rd_seek(g_trackViewMotion.roadIndex);
    rd_project_behind();
    rd_seek(g_trackViewMotion.roadIndex);
    deadline = g_tick + 9;
    vw_draw();
    while (deadline > g_tick)
        ;
}


void rd_advance(void)
{
    enum TerrainFlags narrow;
    int goal6, diff;

    g_trackViewMotion.roadIndex += g_trackViewMotion.roadFraction >> 3;
    rd_seek(g_trackViewMotion.roadIndex);
    g_trackViewMotion.roadFraction &= 7;
    narrow = *g_hereFlagsPtr & TERRAIN_WIDTH_MASK;
    if (narrow) {
        if (narrow & TERRAIN_NARROW) {
            g_leftEdgeX = 2000;
            g_edgeXR = 0xaf0;
        } else if (narrow & TERRAIN_LEFT_CLOSED) {
            g_leftEdgeX = 2000;
            g_edgeXR = 0xce4;
        } else {
            g_leftEdgeX = 1500;
            g_edgeXR = 0xaf0;
        }
    } else {
        g_leftEdgeX = 1500;
        g_edgeXR = 0xce4;
    }
    narrow = g_hereFlagsPtr[4] & TERRAIN_WIDTH_MASK;
    if (narrow) {
        if (narrow & TERRAIN_NARROW) {
            g_edgesRoad.leftEdge = 2000;
            g_trackViewMotion.rightEdge = 0xaf0;
        } else if (narrow & TERRAIN_LEFT_CLOSED) {
            g_edgesRoad.leftEdge = 2000;
            g_trackViewMotion.rightEdge = 0xce4;
        } else {
            g_edgesRoad.leftEdge = 1500;
            g_trackViewMotion.rightEdge = 0xaf0;
        }
    } else {
        g_edgesRoad.leftEdge = 1500;
        g_trackViewMotion.rightEdge = 0xce4;
    }
    g_trackViewMotion.roadHeight = *g_heightRoadPtr;
    if (g_demo != 0 || g_autoDrive != 0 ||
        (g_driver.raceType == RACE_DRAG ? DRAG_FINISH_INDEX : ROAD_FINISH_INDEX) < g_trackViewMotion.roadIndex) {
        goal6 = 0x960;
        if (g_roadOppPos.lateralX > 0x960)
            goal6 -= 0x140;
        else
            goal6 += 0xa0;
        diff = g_trackViewMotion.lateralX - goal6;
        if (abs(diff) > 100)
            g_trackViewMotion.lateralX = goal6 + (goal6 < g_trackViewMotion.lateralX ? 100 : -100);
        else
            g_trackViewMotion.lateralX = goal6;
        if (abs(*g_hereCurvePtr - g_trackViewMotion.curveX) > 0) {
            g_driveEng.steering = (g_trackViewMotion.curveX - *g_hereCurvePtr) * 2;
            if (abs(g_driveEng.steering) > 24)
                g_driveEng.steering = g_driveEng.steering > 0 ? 24 : -24;
        }
        g_trackViewMotion.curveX = *g_hereCurvePtr;
    }
}

/* Returns 1 when the car at the current terrain index would hit something. */

int rd_check_crash(void)
{
    g_onShoulder = 0;
    if (g_leftEdgeX - 0x104 < g_trackViewMotion.lateralX - 0x1e &&
        g_trackViewMotion.lateralX + 0x46 < g_edgeXR + 0x104) {
        if (g_leftEdgeX > g_trackViewMotion.lateralX - 0x1e) {
            if ((g_terrain0[g_trackViewMotion.roadIndex] & ROADSIDE_BARRICADE) ||
                (g_terrain0[g_trackViewMotion.roadIndex - 1] & ROADSIDE_BARRICADE))
                return 1;
            if (g_terrain3[g_trackViewMotion.roadIndex + 1] & TERRAIN_GANTRY_MASK)
                return 1;
            g_onShoulder = 1;
        } else if (g_trackViewMotion.lateralX + 0x46 > g_edgeXR) {
            if ((g_terrain1[g_trackViewMotion.roadIndex] & ROADSIDE_BARRICADE) ||
                (g_terrain1[g_trackViewMotion.roadIndex - 1] & ROADSIDE_BARRICADE))
                return 1;
            if (g_terrain3[g_trackViewMotion.roadIndex + 1] & (TERRAIN_GANTRY_MASK | TERRAIN_POLE))
                return 1;
            g_onShoulder = 1;
        }
        return 0;
    }
    return 1;
}


void rd_start(void)
{
    g_turnRight = g_turnLeft = 0;
    rd_reset();
    vw_start();
}


void rd_turn(int dir)
{
    if (dir == 1)
        g_trackViewMotion.curveX += 3;
    else
        g_trackViewMotion.curveX -= 3;
}


void rd_update(void)
{
    if (g_driveEng.steeringInput && g_driveEng.speed &&
        abs(g_trackViewMotion.curveX - *g_hereCurvePtr - g_driveEng.steeringInput) < 200)
        g_trackViewMotion.curveX -= (g_driveEng.steeringInput >> 1) + 1;
    if (!g_runRacingFlag) {
        if (g_hasSlowCpu)
            g_trackViewMotion.distanceStep = 6;
        else
            g_trackViewMotion.distanceStep = 4;
    }
    if (g_trackViewMotion.distanceStep > 0 && g_startDrawRows > 0)
        g_startDrawRows -= 4;
    rd_draw_frame();
    if (g_demo == 0 && g_autoDrive == 0) {
        if (g_failureTire)
            g_trackViewMotion.lateralX += 25;
        else if (g_driveEng.speed)
            g_trackViewMotion.lateralX -=
                (g_trackViewMotion.curveX - *g_hereCurvePtr) * (g_trackViewMotion.distanceStep + g_driveEng.tireSlip) >>
                2;
    }
    if (!g_runRacingFlag) {
        g_trackViewMotion.roadFraction += g_trackViewMotion.distanceStep;
        if (g_trackViewMotion.roadFraction >= 8)
            rd_advance();
        if ((g_terrain0[g_trackViewMotion.roadIndex + 7] & ROADSIDE_DESTINATION) || g_turnLeft) {
            g_turnLeft = 1;
            rd_turn(1);
        }
        if ((g_terrain1[g_trackViewMotion.roadIndex + 6] & ROADSIDE_DESTINATION) || g_turnRight) {
            rd_turn(-1);
            g_turnRight = 1;
        }
    }
}
