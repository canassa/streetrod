/* SCENERY.H - declarations of what SCENERY.C defines. */

#ifndef SCENERY_H
#define SCENERY_H


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
