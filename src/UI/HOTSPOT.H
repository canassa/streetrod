/* HOTSPOT.H - declarations of what HOTSPOT.C defines. */

#ifndef HOTSPOT_H
#define HOTSPOT_H
#include "STRUCTS.H"

extern struct Hot g_hots[];                             /* DS:09F4 */
extern struct Hot g_hotPageRows[];                      /* DS:0F82 */
extern struct Hot g_hotBuy[];                           /* DS:11B0 */
extern struct Hot g_gar[];                             /* DS:121C */
extern struct Hot g_hotCalendar[];                             /* DS:134E */
extern struct Hot g_hotTransBolts[];                             /* DS:1804 */
extern struct Hot g_engineHots[];                             /* DS:184C */
extern struct Hot g_engineHotTemplate[];                             /* DS:1948 */
extern struct Hot g_hotGasStation[];                    /* DS:19EA */
extern struct Hot g_hotGasExit[];                             /* DS:1A44 */
extern struct Hot g_hotRepair[];                        /* DS:1AC2 */
extern struct Hot g_hotReadyBox[];                      /* DS:1D80 */
extern struct Hot g_hotBodyChange[];                             /* DS:1E34 */
extern struct Hot g_hotBobs[];                          /* DS:20AA */
extern char fz79a8[2];                                  /* DS:79A8 */
extern char ah7aa4w[6];                                 /* DS:7AA4 */
extern char bz7ee2[64];                                 /* DS:7EE2 */
extern char g7ff6f[2];                                  /* DS:7FF6 */
extern char a8ef4fv[2];                                 /* DS:8EF4 */

#endif
