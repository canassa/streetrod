/* SCENERY.H - declarations of what SCENERY.C defines. */

#ifndef SCENERY_H
#define SCENERY_H
#include "STRUCTS.H"

extern int g_5ec6;                                      /* DS:5EC6 */
extern struct Run g_sprTreeLod[7];                      /* DS:5EC8 */
extern struct List g_sprTree;                           /* DS:5EF2 */
extern struct Run g_sprPostLod[7];                      /* DS:5EF6 */
extern struct List g_sprPost;                           /* DS:5F20 */
extern struct Run g_sprHydrantLod[8];                   /* DS:5F24 */
extern struct List g_sprHydrant;                        /* DS:5F54 */
extern struct Run g_sprWarnSignLod[7];                  /* DS:5F58 */
extern struct List g_sprWarnSign;                       /* DS:5F82 */
extern struct Run g_sprBushLod[7];                      /* DS:5F86 */
extern struct List g_sprBush;                           /* DS:5FB0 */
extern struct Run g_sprSpeedSignLod[7];                 /* DS:5FB4 */
extern struct List g_sprSpeedSign;                      /* DS:5FDE */
extern struct Run g_sprBarricadeLod[7];                 /* DS:5FE2 */
extern struct List g_sprBarricade;                      /* DS:600C */
extern struct Run g_6010[7];                            /* DS:6010 */
extern struct List g_603a;                              /* DS:603A */
extern struct Run g_603e[7];                            /* DS:603E */
extern struct List g_6068;                              /* DS:6068 */
extern struct Run g_sprDotLod[1];                       /* DS:606C */
extern struct List g_sprDot;                            /* DS:6072 */
extern int g_6076;                                      /* DS:6076 */
extern struct List *g_scenery[];                        /* DS:6078 */

#endif
