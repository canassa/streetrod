/* F_195F3: set up the sprite g_7d18 from n frame images: the ring of frames, the display pages and the clipped rectangle */

#include "GLOBALS.H"
#include "PROTOS.H"
#include "STRUCTS.H"

struct Sprite *F_195F3(int x, int y, struct Bitmap far **imgs, char (*cell)[8], int n, struct Bitmap far *img,
    struct Bitmap far *save, struct Rect *a8, struct Rect *a9)
{
    int i;

    y -= (*imgs)->height - 1;
    for (i = 0; i < n; i++) {
        g_77ba[i].pos = (long)*imgs++;
        if (cell)
            g_77ba[i].data = g_vid.mode == -2 ? cell++ : &g_6e68[i];
        else
            g_77ba[i].data = 0;
        g_77ba[i].nxt[0] = &g_77ba[i + 1];
    }
    g_7d18.frame = g_77ba[i - 1].nxt[0] = g_77ba;
    for (i = 0; i < n; i++)
        g_77ba[i].nxt[1] = g_77ba[i].nxt[0]->nxt[0];
    for (i = 0; i < n; i++)
        g_77ba[i].nxt[2] = g_77ba[i].nxt[1]->nxt[1];
    g_7d18.img = img;
    g_7d18.x = x;
    g_7d18.y = y;
    g_7d18.save = save;
    g_7d18.rc[0] = a8;
    g_7d18.rc[1] = a9;
    if (g_vid.mode == -2) {
        for (i = 0; i < 2; i++) {
            g_7d42[i].bm = g_7316[i];
            g_7d42[i].idx = i;
            g_7d42[i].r.x1 = g_7d42[i].r.y1 = 0;
            g_7d42[i].r.x2 = g_7d42[i].r.y2 = -1;
            g_7d42[i].link = &g_7d42[i + 1];
        }
        g_7d42[i - 1].link = g_7d42;
    } else {
        g_7d42[0].bm = g_vid.front;
        g_7d42[0].idx = 0;
        g_7d42[0].r.x1 = g_7d42[0].r.y1 = 0;
        g_7d42[0].r.x2 = g_7d42[0].r.y2 = -1;
        g_7d42[0].link = g_7d42;
    }
    g_7d18.page = g_7d42;
    F_1956E(((struct Bitmap far *)g_7d18.frame->pos)->width,
        ((struct Bitmap far *)g_7d18.frame->pos)->height, g_7d18.x, g_7d18.y,
        g_7d42[0].bm->width, g_7d42[0].bm->height, &g_7d42[0].r);
    g_7d18.seq = g_764e;
    return &g_7d18;
}
