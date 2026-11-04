/* MUSIC.H - declarations of what MUSIC.C defines. */

#ifndef MUSIC_H
#define MUSIC_H
#include "STRUCTS.H"

extern int g_toneShapes[][8];
extern struct Tone g_tones[];
void mu_play(void far *song, void *tones);
void mu_set_vibrato(int on, int a, int b, int c, int d);
void mu_suspend(int on);

#endif
