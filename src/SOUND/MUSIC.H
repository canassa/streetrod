/* MUSIC.H - declarations of what MUSIC.C defines. */

#ifndef MUSIC_H
#define MUSIC_H
#include "STRUCTS.H"

extern int g_toneShapes[][8];                           /* DS:57F6 */
extern struct Tone g_tones[];                           /* DS:5826 */
void mu_play(void far *song, void *tones);           /* 0x1628A */
void mu_set_vibrato(int on, enum Waveform waveform, int length, int depth, int delay);/* 0x1633D */
void mu_suspend(int on);                                   /* 0x16381 */

#endif
