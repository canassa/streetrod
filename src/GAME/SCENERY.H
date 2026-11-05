/* SCENERY.H - declarations of what SCENERY.C defines. */

#ifndef SCENERY_H
#define SCENERY_H


enum SceneryId {
    SCENERY_TREE = 0,
    SCENERY_POST = 1,
    SCENERY_HYDRANT = 2,
    SCENERY_WARNING_SIGN = 3,
    SCENERY_BUSH = 4,
    SCENERY_SPEED_SIGN = 5,
    SCENERY_BARRICADE = 6,
    SCENERY_SMALL_ROCK = 7,
    SCENERY_LARGE_ROCK = 8,
    SCENERY_POLE = 9,
    SCENERY_TREE_2 = 10,
    SCENERY_TREE_3 = 11,
    SCENERY_ARROW_LEFT = 12,
    SCENERY_CITY_LIMITS = 13,
    SCENERY_ARROW_UP = 14,
    SCENERY_NARROW_SIGN = 15,
    SCENERY_CHEVRON_LEFT = 16,
    SCENERY_CHEVRON_RIGHT = 17,
    SCENERY_S_CURVE_SIGN = 18,
    SCENERY_S_CURVE_SIGN_2 = 19,
    SCENERY_DOT = 20
};

struct Part {
    signed char dx;
    signed char dy;
    signed char w;
    signed char color;
};

struct Run {
    int n;
    struct Part far *parts;
};

struct List {
    int last;
    struct Run *items;
};

extern int g_unused22;
extern struct List *g_scenery[];

extern struct Run g_sprTreeLod[7];

extern struct Run g_sprPostLod[7];

extern struct Run g_sprHydrantLod[8];

extern struct Run g_sprWarnSignLod[7];

extern struct Run g_sprBushLod[7];

extern struct Run g_sprSpeedSignLod[7];

extern struct Run g_sprBarricadeLod[7];

extern struct Run g_sprSmallRockLod[7];

extern struct Run g_sprLargeRockLod[7];

extern struct Run g_sprDotLod[1];

#endif
