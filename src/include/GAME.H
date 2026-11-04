/* GAME.H - declarations of what GAME.C defines. */

#ifndef GAME_H
#define GAME_H
#include "STRUCTS.H"

extern struct Car *g_4f46;                              /* DS:4F46 */
extern struct Car *g_4f48;                              /* DS:4F48 */
extern int g_4f4a;                                      /* DS:4F4A */
extern int g_picsFreed;                                 /* DS:4F4C */
extern int g_paperPics[];                               /* DS:4F4E */
extern int g_paperPick;                                 /* DS:4F58 */
extern int g_paperDay;                                  /* DS:4F5A */
extern char *g_offerText;                               /* DS:4FA4 */
void game_pause(int a);                                 /* 0x03A83 */
void game_message(int n);                               /* 0x03AB8 */
struct Node * near game_new_part(void);                 /* 0x03C1B */
int game_part_value(struct Node *n);                    /* 0x03C5A */
struct Node2 * near game_new_car(void);                 /* 0x03FB1 */
struct Node * near game_add_part(int idx, int a);       /* 0x03FF0 */
struct Car * near game_make_car(int idx, int value, struct Car *old);/* 0x04036 */
int near game_take_part(struct Node *n, int keep);      /* 0x04386 */
int near game_scrap_car(struct Car *c, int keep);       /* 0x043BF */
int game_car_look(struct Car *c);                       /* 0x049A2 */
void game_play_anim(int h);                             /* 0x04FCA */
void game_popup(int a, int b, int c);                   /* 0x04FFA */
void near game_run(void);                               /* 0x0503F */

#endif
