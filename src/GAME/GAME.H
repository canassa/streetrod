/* GAME.H - declarations of what GAME.C defines. */

#ifndef GAME_H
#define GAME_H
#include "STRUCTS.H"

extern struct Car *g_old;
extern struct Car *g_new;
extern int g_4f4a;
extern int g_picsFreed;
extern int g_paperPics[];
extern int g_paperPick;
extern int g_paperDay;
extern char *g_offerText;
void gm_pause(int a);
void gm_message(int n);
struct Node * near gm_new_part(void);
int gm_part_value(struct Node *n);
struct Node2 * near gm_new_car(void);
struct Node * near gm_add_part(int idx, int a);
struct Car * near gm_make_car(int idx, int value, struct Car *old);
int near gm_take_part(struct Node *n, int keep);
int near gm_scrap_car(struct Car *c, int keep);
int gm_car_look(struct Car *c);
void gm_play_anim(int h);
void gm_popup(int a, int b, int c);
void near gm_run(void);

#endif
