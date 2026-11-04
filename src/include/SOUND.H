/* SOUND.H - declarations of what SOUND.C defines. */

#ifndef SOUND_H
#define SOUND_H
#include "STRUCTS.H"

extern int g_songRepeats[];                             /* DS:5858 */
extern char g_jingleRepeats[];                          /* DS:587E */
extern int g_songTones[];                               /* DS:5882 */
extern int g_jingleTones[];                             /* DS:58CC */
extern int g_musicOn;                                   /* DS:58D0 */
extern struct Dev *g_voice;                             /* DS:58D2 */
extern int g_voiceNum;                                  /* DS:58D4 */
extern int g_savedMusicOn;                              /* DS:58D6 */
extern int g_musicSuspended;                            /* DS:58D8 */
extern int g_squealOn;                                  /* DS:58DA */
extern int g_sirenOn;                                   /* DS:58DC */
int sound_calibrate(int n);                             /* 0x163BE */
void sound_set_freq(long freq);                         /* 0x16455 */
void sound_init(void);                                  /* 0x16481 */
void sound_on(void);                                    /* 0x164A2 */
void sound_off(void);                                   /* 0x164BD */
void F_168C8(int on);                                   /* 0x168C8 */
void F_168F3(int on);                                   /* 0x168F3 */
void F_16943(int on);                                   /* 0x16943 */
void F_16993(int on);                                   /* 0x16993 */
void F_169E1(int on);                                   /* 0x169E1 */
void F_16A2F(int on);                                   /* 0x16A2F */
void F_16A5A(int on);                                   /* 0x16A5A */
void sound_siren(int on);                               /* 0x16AA8 */
void sound_squeal(int on);                              /* 0x16AC1 */
void sound_buzz(void);                                  /* 0x16B3A */
void F_16B61(void);                                     /* 0x16B61 */
void F_16B97(void);                                     /* 0x16B97 */
void F_16BC1(void);                                     /* 0x16BC1 */
void sound_crash(void);                                 /* 0x16BE3 */
void F_16C8F(void);                                     /* 0x16C8F */
void F_16CF3(void);                                     /* 0x16CF3 */

#endif
