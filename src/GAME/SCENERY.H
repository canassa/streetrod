/**
 * @file SCENERY.H
 * @brief Distance-scaled roadside sprite tables.
 */

#ifndef SCENERY_H
#define SCENERY_H


/**
 * @brief Indices into the roadside sprite lists, each ordered by distance.
 *
 * The dot sprite also draws the dashboard steering marker.
 */
enum SceneryId {
    SCENERY_TREE = 0, /**< Town curb or country outer verge. */
    SCENERY_POST = 1, /**< Country road-edge marker. */
    /** Town curb; also uses a special near-distance drawing path. */
    SCENERY_HYDRANT = 2,
    SCENERY_WARNING_SIGN = 3,   /**< At the road edge before a barricade. */
    SCENERY_BUSH = 4,           /**< Town curb, country verge or beyond the verge. */
    SCENERY_SPEED_SIGN = 5,     /**< Right road edge after the drag finish. */
    SCENERY_BARRICADE = 6,      /**< At the closed road edge. */
    SCENERY_SMALL_ROCK = 7,     /**< Country road edge, facing inward. */
    SCENERY_LARGE_ROCK = 8,     /**< Country road edge, facing inward. */
    SCENERY_POLE = 9,           /**< Right country road edge. */
    SCENERY_TREE_2 = 10,        /**< Country position beyond the outer verge. */
    SCENERY_TREE_3 = 11,        /**< Country outer-verge position, varying by side. */
    SCENERY_ARROW_LEFT = 12,    /**< Country overhead gantry face. */
    SCENERY_CITY_LIMITS = 13,   /**< Overhead drag-finish sign. */
    SCENERY_ARROW_UP = 14,      /**< Country overhead straight-ahead gantry face. */
    SCENERY_NARROW_SIGN = 15,   /**< Right road edge before a narrowed section. */
    SCENERY_CHEVRON_LEFT = 16,  /**< Right road edge on a left bend. */
    SCENERY_CHEVRON_RIGHT = 17, /**< Right road edge on a right bend. */
    /** Right road edge before a left-then-right bend. */
    SCENERY_S_CURVE_SIGN = 18,
    /** Right road edge before a right-then-left bend. */
    SCENERY_S_CURVE_SIGN_2 = 19,
    SCENERY_DOT = 20 /**< Dashboard steering marker; one size only. */
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
