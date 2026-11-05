/* POPUP.H - declarations of what POPUP.C defines. */

#ifndef POPUP_H
#define POPUP_H

struct Bitmap;

struct Win {
    int bg;
    int lines;
    int x;
    int y;
    int w;
    int h;
    int tx;
    int ty;
    int next;
};

struct Bitmap far *pu_grab(int x, int y, int w, int h, struct Bitmap far *src);
void pu_put(struct Bitmap far *bm, int x, int y);
void pu_show(struct Bitmap far *bm, int x, int y);
void pu_open(int n);
void pu_open_at(int n, int x, int y);
void pu_close(void);

#endif
