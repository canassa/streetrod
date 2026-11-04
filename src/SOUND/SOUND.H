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
int so_calibrate(int n);                             /* 0x163BE */
void so_set_freq(long freq);                         /* 0x16455 */
void so_init(void);                                  /* 0x16481 */
void so_on(void);                                    /* 0x164A2 */
void so_off(void);                                   /* 0x164BD */
void so_wheel_roll(int on);                                   /* 0x168C8 */
void so_sticker(int on);                                   /* 0x168F3 */
void F_16943(int on);                                   /* 0x16943 */
void so_pump(int on);                                   /* 0x16993 */
void so_jack(int on);                                   /* 0x169E1 */
void F_16A2F(int on);                                   /* 0x16A2F */
void so_part_move(int on);                                   /* 0x16A5A */
void so_siren(int on);                               /* 0x16AA8 */
void so_squeal(int on);                              /* 0x16AC1 */
void so_buzz(void);                                  /* 0x16B3A */
void so_bolt(void);                                     /* 0x16B61 */
void so_part_set(void);                                     /* 0x16B97 */
void so_body_change(void);                                     /* 0x16BC1 */
void so_crash(void);                                 /* 0x16BE3 */
void so_bump(void);                                     /* 0x16C8F */
void so_bolt_turn(void);                                     /* 0x16CF3 */

#endif
