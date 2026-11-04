#include "GLOBALS.H"
#include "PROTOS.H"

int g_5e14[5][4] = {{1003, 1004, 1057, 1091}, {1010, 1012, 1011, 1239}, {1010, 1247, 1248, 1092}, {1010, 1249, 1250, 1251}, {1010, 1253, 1252, 1254}};   /* DS:5E14 */
int g_5e3c = 0;   /* DS:5E3C */
int g_5e3e = 1;   /* DS:5E3E */
int g_5e40 = 2;   /* DS:5E40 */
struct Blt12 g_5e42 = {320, 82, 0, 18, 0, 18};   /* DS:5E42 */
int g_clip_xmin = 0;   /* DS:5E4E */
int g_clip_ymin = 18;   /* DS:5E50 */
int g_clip_ymax = 101;   /* DS:5E52 */
int g_clip_xmax = 319;   /* DS:5E54 */
unsigned char g_5e56[3][4] = {{87, 37, 34, 0}, {247, 117, 82, 0}, {117, 84, 33, 0}};   /* DS:5E56 */
int g_5e62[18] = {1258, 1237, 1236, 1238, 8, 6, 5, 4, 3, 2, 2, 2, 1, 1, 1, 0, 0, 0};   /* DS:5E62 */
int g_5e86 = 0;   /* DS:5E86 */
int g_5e88[5] = {0, 0, 0, 0, 0};   /* DS:5E88 */
int g_5e92 = 0;   /* DS:5E92 */
int g_5e94[4] = {0, 0, 0, 0};   /* DS:5E94 */
struct Ball g_5e9c[7] = {{256, 8, 8}, {240, 16, 13}, {208, 32, 19}, {168, 40, 24}, {120, 48, 30}, {64, 56, 37}, {0, 64, 43}};   /* DS:5E9C */

/* F_2869E @ 2645:224E (linear 0x2869E, 58 bytes) */


static void near F_2869E(void)
{
    if (g_vid.mode == -2) {
        g_787a = (g_5e3e == 1) ? 0xa200 : 0xa400;
        F_2322E(g_5e40);
    } else {
        g_787a = g_8188->bits.w.seg;
        g_767c = g_8188->bits.w.off;
    }
}
