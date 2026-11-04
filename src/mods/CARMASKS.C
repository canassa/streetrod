#include "STRUCTS.H"
#include "CARMASKS.H"
#include "CARMASK2.H"
#include "PARTS1.H"

int g_5ec6 = 0;   /* DS:5EC6 */
struct List g_5ef2 = {7, g_5ec8};   /* DS:5EF2 */
struct Run g_5ec8[7] = {{13, g_parts1}, {22, g_parts1 + 13}, {47, g_parts1 + 35}, {88, g_parts1 + 82}, {127, g_parts1 + 170}, {165, g_parts1 + 297}, {245, g_parts1 + 462}};   /* DS:5EC8 */
struct List g_5f20 = {7, g_5ef6};   /* DS:5F20 */
struct Run g_5ef6[7] = {{2, g_parts1 + 707}, {2, g_parts1 + 709}, {4, g_parts1 + 711}, {6, g_parts1 + 715}, {9, g_parts1 + 721}, {11, g_parts1 + 730}, {14, g_parts1 + 741}};   /* DS:5EF6 */
struct List g_5f54 = {8, g_5f24};   /* DS:5F54 */
struct Run g_5f24[8] = {{15, g_parts1 + 755}, {20, g_parts1 + 770}, {36, g_parts1 + 790}, {43, g_parts1 + 826}, {49, g_parts1 + 869}, {64, g_parts1 + 918}, {73, g_parts1 + 982}, {88, g_parts1 + 1055}};   /* DS:5F24 */
struct List g_5f82 = {7, g_5f58};   /* DS:5F82 */
struct Run g_5f58[7] = {{10, g_parts1 + 1143}, {25, g_parts1 + 1153}, {33, g_parts1 + 1178}, {56, g_parts1 + 1211}, {97, g_parts1 + 1267}, {156, g_parts1 + 1364}, {165, g_parts1 + 1520}};   /* DS:5F58 */
struct List g_5fb0 = {7, g_5f86};   /* DS:5FB0 */
struct Run g_5f86[7] = {{3, g_parts1 + 1685}, {5, g_parts1 + 1688}, {10, g_parts1 + 1693}, {22, g_parts1 + 1703}, {38, g_parts1 + 1725}, {53, g_parts1 + 1763}, {65, g_parts1 + 1816}};   /* DS:5F86 */
struct List g_5fde = {7, g_5fb4};   /* DS:5FDE */
struct Run g_5fb4[7] = {{17, g_parts1 + 1881}, {31, g_parts1 + 1898}, {47, g_parts1 + 1929}, {75, g_parts1 + 1976}, {109, g_parts1 + 2051}, {175, g_parts1 + 2160}, {211, g_parts1 + 2335}};   /* DS:5FB4 */
struct List g_600c = {7, g_5fe2};   /* DS:600C */
struct Run g_5fe2[7] = {{20, g_parts1 + 2546}, {40, g_parts1 + 2566}, {62, g_parts1 + 2606}, {89, g_parts1 + 2668}, {114, g_parts1 + 2757}, {137, g_parts1 + 2871}, {155, g_parts1 + 3008}};   /* DS:5FE2 */
struct List g_603a = {7, g_6010};   /* DS:603A */
struct Run g_6010[7] = {{7, g_parts1 + 3163}, {7, g_parts1 + 3163}, {7, g_parts1 + 3163}, {12, g_parts1 + 3170}, {18, g_parts1 + 3182}, {32, g_parts1 + 3200}, {46, g_parts1 + 3232}};   /* DS:6010 */
struct List g_6068 = {7, g_603e};   /* DS:6068 */
struct Run g_603e[7] = {{22, g_parts1 + 3278}, {22, g_parts1 + 3278}, {22, g_parts1 + 3278}, {46, g_parts1 + 3300}, {59, g_parts1 + 3346}, {71, g_parts1 + 3405}, {95, g_parts1 + 3476}};   /* DS:603E */
struct List g_6072 = {1, g_606c};   /* DS:6072 */
struct Run g_606c[1] = {{5, g_parts1 + 3571}};   /* DS:606C */
int g_6076 = 15;   /* DS:6076 */
struct List *g_6078[21] = {&g_5ef2, &g_5f20, &g_5f54, &g_5f82, &g_5fb0, &g_5fde, &g_600c, &g_603a, &g_6068, &g_60cc, &g_60fa, &g_6128, &g_6156, &g_6184, &g_61b2, &g_61e0, &g_620e, &g_623c, &g_626a, &g_6298, &g_6072};   /* DS:6078 */
