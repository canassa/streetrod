/* MUSIC.H - declarations of what MUSIC.C defines. */

#ifndef MUSIC_H
#define MUSIC_H
#include "STRUCTS.H"

extern int g_toneShapes[][8];                           /* DS:57F6 */
extern struct Tone g_tones[];                           /* DS:5826 */
void music_play(void far *song, void *tones);           /* 0x1628A */
void music_set_vibrato(int on, int a, int b, int c, int d);/* 0x1633D */
void F_16381(int on);                                   /* 0x16381 */

#endif
