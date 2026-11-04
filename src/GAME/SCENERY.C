#include "STRUCTS.H"
#include "SCENERY.H"
#include "SCENERY2.H"
#include "SPANS1.H"

int g_5ec6 = 0;
struct List g_sprTree = {7, g_sprTreeLod};
struct Run g_sprTreeLod[7] = {{13, g_spans1}, {22, g_spans1 + 13}, {47, g_spans1 + 35}, {88, g_spans1 + 82}, {127, g_spans1 + 170}, {165, g_spans1 + 297}, {245, g_spans1 + 462}};
struct List g_sprPost = {7, g_sprPostLod};
struct Run g_sprPostLod[7] = {{2, g_spans1 + 707}, {2, g_spans1 + 709}, {4, g_spans1 + 711}, {6, g_spans1 + 715}, {9, g_spans1 + 721}, {11, g_spans1 + 730}, {14, g_spans1 + 741}};
struct List g_sprHydrant = {8, g_sprHydrantLod};
struct Run g_sprHydrantLod[8] = {{15, g_spans1 + 755}, {20, g_spans1 + 770}, {36, g_spans1 + 790}, {43, g_spans1 + 826}, {49, g_spans1 + 869}, {64, g_spans1 + 918}, {73, g_spans1 + 982}, {88, g_spans1 + 1055}};
struct List g_sprWarnSign = {7, g_sprWarnSignLod};
struct Run g_sprWarnSignLod[7] = {{10, g_spans1 + 1143}, {25, g_spans1 + 1153}, {33, g_spans1 + 1178}, {56, g_spans1 + 1211}, {97, g_spans1 + 1267}, {156, g_spans1 + 1364}, {165, g_spans1 + 1520}};
struct List g_sprBush = {7, g_sprBushLod};
struct Run g_sprBushLod[7] = {{3, g_spans1 + 1685}, {5, g_spans1 + 1688}, {10, g_spans1 + 1693}, {22, g_spans1 + 1703}, {38, g_spans1 + 1725}, {53, g_spans1 + 1763}, {65, g_spans1 + 1816}};
struct List g_sprSpeedSign = {7, g_sprSpeedSignLod};
struct Run g_sprSpeedSignLod[7] = {{17, g_spans1 + 1881}, {31, g_spans1 + 1898}, {47, g_spans1 + 1929}, {75, g_spans1 + 1976}, {109, g_spans1 + 2051}, {175, g_spans1 + 2160}, {211, g_spans1 + 2335}};
struct List g_sprBarricade = {7, g_sprBarricadeLod};
struct Run g_sprBarricadeLod[7] = {{20, g_spans1 + 2546}, {40, g_spans1 + 2566}, {62, g_spans1 + 2606}, {89, g_spans1 + 2668}, {114, g_spans1 + 2757}, {137, g_spans1 + 2871}, {155, g_spans1 + 3008}};
struct List g_sprSmallRock = {7, g_sprSmallRockLod};
struct Run g_sprSmallRockLod[7] = {{7, g_spans1 + 3163}, {7, g_spans1 + 3163}, {7, g_spans1 + 3163}, {12, g_spans1 + 3170}, {18, g_spans1 + 3182}, {32, g_spans1 + 3200}, {46, g_spans1 + 3232}};
struct List g_sprLargeRock = {7, g_sprLargeRockLod};
struct Run g_sprLargeRockLod[7] = {{22, g_spans1 + 3278}, {22, g_spans1 + 3278}, {22, g_spans1 + 3278}, {46, g_spans1 + 3300}, {59, g_spans1 + 3346}, {71, g_spans1 + 3405}, {95, g_spans1 + 3476}};
struct List g_sprDot = {1, g_sprDotLod};
struct Run g_sprDotLod[1] = {{5, g_spans1 + 3571}};
int g_6076 = 15;
struct List *g_scenery[21] = {&g_sprTree, &g_sprPost, &g_sprHydrant, &g_sprWarnSign, &g_sprBush, &g_sprSpeedSign, &g_sprBarricade, &g_sprSmallRock, &g_sprLargeRock, &g_sprPole, &g_sprTree2, &g_sprTree3, &g_sprArrowSignL, &g_sprCityLimits, &g_sprArrowSignUp, &g_sprNarrowSign, &g_sprChevronL, &g_sprChevronR, &g_sprSCurveSign, &g_sprSCurveSign2, &g_sprDot};
