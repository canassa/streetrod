/* SPEAKER.H - declarations of what SPEAKER.ASM defines. */

#ifndef SPEAKER_H
#define SPEAKER_H

extern int g_tunePtr;
extern int g_toneDelay;
void sk_delay(int n);
void sk_play(int *table);

#endif
