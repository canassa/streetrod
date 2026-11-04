/* SYS.H - declarations of what SYS.C defines. */

#ifndef SYS_H
#define SYS_H
#include "STRUCTS.H"

extern int g_timerFirst;                                /* DS:0566 */
extern int g_timerHead;                                 /* DS:0568 */
extern int g_timerFree;                                 /* DS:056A */
extern long g_timerNext;                                /* DS:056C */
extern char g_0570;                                     /* DS:0570 */
extern char g_timerBusy;                                /* DS:0571 */
extern struct Link g_timers[];                          /* DS:0572 */
extern int g_05ba;                                      /* DS:05BA */
extern int g_05bc[8];                                   /* DS:05BC */
extern long g_waitUntil;                                /* DS:05CC */
extern int g_anyInput;                                  /* DS:05D0 */
extern int g_inputKey;                                  /* DS:05D2 */
extern int g_05d4;                                      /* DS:05D4 */
extern int g_05d6;                                      /* DS:05D6 */
extern int g_05d8;                                      /* DS:05D8 */
extern int g_05da;                                      /* DS:05DA */
extern int g_msgTicks;                                  /* DS:05DC */
extern int g_clickEnds;                                 /* DS:05DE */
extern int g_kbdHooked;                                 /* DS:05E0 */
extern int g_mouseDx;                                   /* DS:05E2 */
extern int g_mouseDy;                                   /* DS:05E4 */
extern int g_mouseAccX;                                 /* DS:05E6 */
extern int g_mouseAccY;                                 /* DS:05E8 */
extern int g_mouseShiftX;                               /* DS:05EA */
extern int g_mouseShiftY;                               /* DS:05EC */
extern int g_mouseLeft;                                 /* DS:05EE */
extern int g_mouseRight;                                /* DS:05F0 */
extern char g_mousePresent;                             /* DS:05F2 */
extern char g_05f3;                                     /* DS:05F3 */
extern int g_05f4[2];                                   /* DS:05F4 */
extern long g_tick;                                     /* DS:05F8 */
extern long g_05fc;                                     /* DS:05FC */
extern long g_0600;                                     /* DS:0600 */
extern long g_0604;                                     /* DS:0604 */
extern char g_tickDiv;                                  /* DS:0608 */
extern char g_driveDiv;                                 /* DS:0609 */
extern int g_060a;                                      /* DS:060A */
extern char *g_errRead;                                 /* DS:060C */
extern char *g_errWriteProt;                            /* DS:060E */
extern int g_demo;                                      /* DS:0610 */
extern int g_autoDrive;                                 /* DS:0612 */
extern char g_064d;                                     /* DS:064D */
extern char g_keymapGroup;                              /* DS:064E */
extern struct KeyMap g_keymapSaved[];                   /* DS:0650 */
extern char g_protectPal[];                             /* DS:0692 */
extern int g_06a2;                                      /* DS:06A2 */
extern int g_demoQuit;                                  /* DS:06A4 */
extern char g_demoKeyDown;                              /* DS:06A6 */
extern char g_demoMoving;                               /* DS:06A7 */
extern int g_demoCount;                                 /* DS:06A8 */
extern int g_demoLastGroup;                             /* DS:06AA */
extern int g_06ac;                                      /* DS:06AC */
extern int g_demoTargetX;                               /* DS:06AE */
extern int g_demoTargetY;                               /* DS:06B0 */
extern int g_demoScriptPos;                             /* DS:06B2 */
extern int g_demoScript[];                              /* DS:06B4 */
extern char g_cursorBusy;                               /* DS:06E0 */
extern int g_vibCount;                                  /* DS:06E2 */
extern int g_06e4;                                      /* DS:06E4 */
extern int g_overRev;                                   /* DS:06E6 */
extern char g_driveBusy;                                /* DS:06E8 */
extern char g_lastFire;                                 /* DS:06E9 */
extern char g_06ea;                                     /* DS:06EA */
extern int g_vibratoActive;                             /* DS:72EC */
extern struct Eng e_78e0;                               /* DS:78E0 */
extern struct Dev bs79aa;                               /* DS:79AA */
extern struct Dev ao79c6t;                              /* DS:79C6 */
extern void (interrupt far *w_7aaa)(void);              /* DS:7AAA */
extern struct State7D2E ao7d2ev;                        /* DS:7D2E */
extern struct State7E8A ar7e8av;                        /* DS:7E8A */
extern char g7f32s[];                                   /* DS:7F32 */
extern int ao8228u;                                     /* DS:8228 */
extern long b_82bc;                                     /* DS:82BC */
extern struct Tone *bn82c4;                             /* DS:82C4 */
extern int z_8b7c;                                      /* DS:8B7C */
extern int g8b8eb;                                      /* DS:8B8E */
extern struct Lims ao8b90v;                             /* DS:8B90 */
extern struct Hot *at8ba8v;                             /* DS:8BA8 */
extern int az8baa;                                      /* DS:8BAA */
extern int cz8bc0y;                                     /* DS:8BC0 */
extern int g8ef6q;                                      /* DS:8EF6 */
extern int g_8ef8;                                      /* DS:8EF8 */
extern int aj8efax;                                     /* DS:8EFA */
extern int bw8efc;                                      /* DS:8EFC */
void sys_hover_reset(void);                             /* 0x00D78 */
void sys_music_pause(void);                             /* 0x00D99 */
void sys_music_resume(void);                            /* 0x00DD8 */
void sys_input_reset(void);                             /* 0x00E24 */
void sys_timer_remove(int n);                           /* 0x00F0B */
void sys_unhook(void);                                  /* 0x00FB7 */
void sys_exit(void);                                    /* 0x00FEA */
void sys_crit_error(unsigned deverr, unsigned char errcode);/* 0x0105B */
void sys_init(void);                                    /* 0x01111 */
void sys_keymap_set(int n);                             /* 0x012DE */
int sys_keymap_restore(int keep);                       /* 0x01391 */
int sys_wait_input(int n);                              /* 0x01417 */
int sys_timer_add(int delay, int pri);                  /* 0x01843 */
void sys_timer_anim(int fc, int delay);                 /* 0x0198A */
void sys_timer_cancel(int n);                           /* 0x019BC */
void sys_music_start(int n, int cnt);                   /* 0x01A05 */
void sys_music_stop(void);                              /* 0x01A7C */
void sys_drive_start(void);                             /* 0x01AAD */
void sys_drive_stop(void);                              /* 0x01B27 */
int sys_sign(int x);                                    /* 0x01B91 */
void sys_demo_step(void);                               /* 0x01BAE */
void interrupt far sys_timer_isr(void);                 /* 0x01FE2 */
void sys_car_halt(void);                                /* 0x02374 */
void interrupt far sys_drive_isr(void);                 /* 0x0238C */

#endif
