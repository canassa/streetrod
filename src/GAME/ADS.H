/* ADS.H - declarations of what ADS.C defines. */

#ifndef ADS_H
#define ADS_H

struct Bitmap;

/* blit request handed to the g_videoOps hooks (20 bytes). The first six ints are the same as
   struct Clip; da_clip_blt reads only those. mx, my: origin in the mask bitmap (tb_compose). */
struct Blt {
    int w;
    int h;
    int sx;
    int sy;
    int dx;
    int dy;
    int fc;
    int fe;
    int mx;
    int my;
};

/* List menu pages (av_open fills them, av_run pages through them): start[k] is the
   first entry of page k, n the page count, cur the current page, list the menu (count, then items). */
struct Pages {
    int start[12];
    int n;
    int cur;
    int *list;
};

extern struct Blt g_adPacket;

extern struct Pages g_pages;
extern struct Bitmap far *g_adPicture;
extern char g_adRedraw;
extern int g_adResume;
extern int g_adIdxEndPageState;
void av_open(int *list, int base);
int av_run(void);
void av_close(void);

#endif
