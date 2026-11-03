/* F_19BFE: slide a one-frame sprite of bitmap bm from y0 to y1 at column x, refreshing the front page under it */
/* Local names are placeholders chosen to reproduce the original stack layout. */

#include "GLOBALS.H"
#include "PROTOS.H"
#include "STRUCTS.H"

void F_19BFE(int a, int b, int x, int y0, int y1, struct Bitmap far *bm)
{
    struct Sprite *sp;
    int n1;
    int dir;
    struct Bitmap far *save;
    struct Bitmap far *img2;
    int old;

    old = g_8b7c;
    n1 = y1 - y0;
    dir = 1;
    if (n1 < 0) {
        n1 = -n1;
        dir = -1;
    }
    n1++;
    img2 = F_11A72(x, 0, bm->width, 200, 0);
    save = F_1934E(img2->width, img2->height, 0, 2);
    F_19ED0(img2, save, 1);
    sp = F_195F3(x, bm->height + y0 - 1, &bm, 0, 1, img2, save, 0, 0);
    F_199DD(sp, 0, n1, 0, (char)dir);
    sp->page->bm = g_vid.back;
    while ((*g_78c6)(sp)) {
        if (g_0792)
            (*g_78aa)(g_0792, g_vid.back, &g_70ac, 1);
        if (g_vid.f16)
            F_1A92A(g_vid.back, g_vid.front, x, a, bm->width + x, b);
    }
    if (g_vid.f16)
        F_1A92A(g_vid.front, g_vid.back, x, (y0 > y1 ? y1 : y0) < 0 ? 0 : (y0 > y1 ? y1 : y0), bm->width + x,
            (y0 < y1 ? y1 : y0) + bm->height > 199 ? 199 : (y0 < y1 ? y1 : y0) + bm->height);
    g_8b7c = old;
    F_19438(2);
}
