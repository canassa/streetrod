/* GARAGE.H - declarations of what GARAGE.C defines. */

#ifndef GARAGE_H
#define GARAGE_H
#include "STRUCTS.H"

extern int g_tirePics[][4];                                 /* DS:5030 */
extern int g_colX;                                      /* DS:5048 */
extern int g_rowY[];                                    /* DS:504A */
extern int g_wOff[];                                    /* DS:5052 */
extern char *g_mon[];                                  /* DS:5082 */
extern int g_hIds[];                                    /* DS:508A */
extern char g_paint[][2];                                /* DS:50C2 */
extern int g_pad[];                                    /* DS:50CE */
extern char *g_iTxt[];                                  /* DS:511E */
extern int g_nCarb[];                                    /* DS:5126 */
extern int g_nBolt[];                                    /* DS:5130 */
extern int g_nextPartCategory[];                                    /* DS:513C */
extern int g_cPos[][6];                                 /* DS:5148 */
extern int g_5184[];                                    /* DS:5184 */
extern int g_bPos[];                                    /* DS:5186 */
extern int g_5198[];                                    /* DS:5198 */
extern int g_bIdx[];                                    /* DS:519A */
extern struct State8272 m_8272;                         /* DS:8272 */
int ga_day(void);                                   /* 0x06476 */
void ga_time(int *a, int *b);                           /* 0x064A3 */
void ga_check_end(int n);                                    /* 0x06529 */
void ga_calendar(int a);                                    /* 0x065AF */
void ga_date(int x);                                    /* 0x06750 */
void ga_palette(int a);                                    /* 0x067CF */
int ga_hotspot(int n);                                     /* 0x06827 */
void ga_clear_name(void);                                     /* 0x06AA6 */
int near ga_menu(int mode);                             /* 0x06B06 */
void near ga_paint(void);                                /* 0x06F6B */
void near ga_body(unsigned mask);                       /* 0x0711A */
void near ga_stickers(void);                                /* 0x07367 */
void near ga_spares(int t, struct Node **list, char *buf, int start);/* 0x076A7 */
void near ga_install(struct Node *n, struct Node **slot);  /* 0x07741 */
struct Node * near ga_unlink(struct Node *n);             /* 0x077B7 */
void near ga_tires(void);                                /* 0x07800 */
void ga_info(void);                                     /* 0x079EB */
void near ga_transmission(void);                                /* 0x07AA4 */
int ga_ready(struct Car *car, int flag);                 /* 0x07EE6 */
void near ga_hood(struct Car *car);                 /* 0x081AB */

#endif
