/* M_02B9A.H - declarations of what M_02B9A.ASM defines. */

#ifndef M_02B9A_H
#define M_02B9A_H
#include "STRUCTS.H"

extern int g_joyX;                                      /* DS:4718 */
extern int g_joyY;                                      /* DS:471A */
extern char g_joyButtons;                               /* DS:471C */
extern char g_joystick;                                 /* DS:471D */
extern int g_joyCenterX;                                /* DS:471E */
extern int g_joyCenterY;                                /* DS:4720 */
extern int g_joyMinX;                                   /* DS:4722 */
extern int g_joyMaxX;                                   /* DS:4724 */
extern int g_joyMinY;                                   /* DS:4726 */
extern int g_joyMaxY;                                   /* DS:4728 */
extern int g_cursorX;                                   /* DS:472A */
extern int g_cursorY;                                   /* DS:472C */
extern char g_keyDx;                                    /* DS:472E */
extern char g_keyDy;                                    /* DS:472F */
extern char g_lastDx;                                   /* DS:4730 */
extern char g_lastDy;                                   /* DS:4731 */
extern char g_buttons;                                  /* DS:4732 */
extern signed char g_4733;                              /* DS:4733 */
extern int g_4734;                                      /* DS:4734 */
extern char g_click;                                    /* DS:4736 */
extern char g_click2;                                   /* DS:4737 */
extern int g_clickX;                                    /* DS:4738 */
extern int g_clickY;                                    /* DS:473A */
extern int g_click2X;                                   /* DS:473C */
extern int g_click2Y;                                   /* DS:473E */
extern int g_inputLock;                                 /* DS:4740 */
extern int g_4742;                                      /* DS:4742 */
extern int g_4744;                                      /* DS:4744 */
extern int g_savedCursorX;                              /* DS:4746 */
extern int g_savedCursorY;                              /* DS:4748 */
extern int g_474a;                                      /* DS:474A */
extern int g_474c[];                                    /* DS:474C */
extern int g_4752[];                                    /* DS:4752 */
extern char g_scanMap[];                                /* DS:4766 */
extern char g_demoKey;                                  /* DS:47CC */
extern char g_fireLatch;                                /* DS:47CD */
extern char g_soundOn;                                  /* DS:47CE */
extern char g_speakerOn;                                /* DS:47CF */
extern char g_lastKey;                                  /* DS:47D0 */
extern char g_47d1[3];                                  /* DS:47D1 */
extern int g_oldInt9Seg;                                /* DS:47D4 */
extern int g_oldInt9Off;                                /* DS:47D6 */
extern int g_dirHeld[8];                                   /* DS:47D8 */
extern int g_joySavedSp;                                      /* DS:47E8 */
extern char g_modeBitsA;                                     /* DS:47EA */
extern char g_biosModeA;                                     /* DS:47EB */
extern char g_modeBitsB;                                     /* DS:47EC */
extern char g_biosModeB;                                     /* DS:47ED */
extern int g_firePressed;                               /* DS:47EE */
extern char g_47f0;                                     /* DS:47F0 */
extern char g_gearText[8];                              /* DS:47FE */
extern struct Item g_4806[];                            /* DS:4806 */
extern int g_partWear[];                                /* DS:4966 */
extern int g_496a;                                      /* DS:496A */
extern int g_496c[2];                                   /* DS:496C */
extern int g_betRaces;                                  /* DS:4970 */
extern int g_dragWins;                                  /* DS:4972 */
extern int g_roadWins;                                  /* DS:4974 */
extern int g_kingDragWon;                               /* DS:4976 */
extern char g_4978[][5];                                /* DS:4978 */
extern char g_4988[][3];                                /* DS:4988 */
extern int g_4998[][5];                                 /* DS:4998 */
extern int g_49ca[];                                    /* DS:49CA */
extern int g_49d2[];                                    /* DS:49D2 */
extern char *g_49dc;                                    /* DS:49DC */
extern char *g_gearLabel;                               /* DS:49DE */
extern int g_list[];                                    /* DS:49E0 */
void m_02b9a_store_mode_a(char a, char b);                           /* 0x02B9A */
void set_g47ec_47ed(char a, char b);                    /* 0x02BAD */

#endif
