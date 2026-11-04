/* VIDEO.H - declarations of what VIDEO.C defines. */

#ifndef VIDEO_H
#define VIDEO_H
#include "STRUCTS.H"

extern struct Style g_03a0;                             /* DS:03A0 */
extern struct Style g_03f0;                             /* DS:03F0 */
extern unsigned char g_palette[];                       /* DS:0440 */
extern int g_0450;                                      /* DS:0450 */
extern char g_0452[16];                                 /* DS:0452 */
extern unsigned char g_0462[16];                        /* DS:0462 */
extern int g_0472;                                      /* DS:0472 */
extern struct Bitmap far *bp7316[];                     /* DS:7316 */
extern struct Obj bm7568[];                             /* DS:7568 */
extern struct Style *bustyle;                           /* DS:787E */
extern int (far *ab78aat)(struct Bitmap far *, struct Bitmap far *, struct Blt *, int);/* DS:78AA */
extern struct Colors x_colors;                          /* DS:8242 */
extern char iz8248;                                     /* DS:8248 */
extern struct Bitmap far *bu825a;                       /* DS:825A */
extern int ac825et;                                     /* DS:825E */
extern int aw8260y;                                     /* DS:8260 */
extern int d_8266;                                      /* DS:8266 */
extern int ap8268t;                                     /* DS:8268 */
void F_10B7C(void);                                     /* 0x10B7C */
void F_10B8C(void);                                     /* 0x10B8C */
void F_10B9C(void);                                     /* 0x10B9C */
void video_swap_pal(void);                              /* 0x10BAC */
void video_init(struct Bitmap far *front, struct Bitmap far *back);/* 0x10BD9 */
void video_anim_stop(int n);                            /* 0x10D6C */
int video_anim_start(int n);                            /* 0x11149 */
void video_anim_resume(int n);                          /* 0x111C1 */
int video_anim_active(int n);                           /* 0x111F2 */
int video_anim_hook(struct Obj *o, struct Cmd *c);      /* 0x11213 */
void F_112CB(int reg, char color);                      /* 0x112CB */
void video_set_palette(char *table);                    /* 0x11324 */

#endif
