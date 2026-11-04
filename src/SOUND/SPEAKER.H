/* SPEAKER.H - declarations of what SPEAKER.ASM defines. */

#ifndef SPEAKER_H
#define SPEAKER_H
#include "STRUCTS.H"

extern int g_tunePtr;                                   /* DS:58DE */
extern int g_toneDelay;                                 /* DS:58E0 */
void speaker_delay(int n);                              /* 0x21962 */
void speaker_play(int *table);                          /* 0x2197D */

#endif
