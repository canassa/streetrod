/* INPUTDAT.H - declarations of what INPUTDAT.ASM defines. */

#ifndef INPUTDAT_H
#define INPUTDAT_H
#include "STRUCTS.H"

extern int g_joyX;
extern int g_joyY;
extern char g_joyButtons;
extern char g_joystick;
extern int g_joyCenterX;
extern int g_joyCenterY;
extern int g_joyMinX;
extern int g_joyMaxX;
extern int g_joyMinY;
extern int g_joyMaxY;
extern int g_cursorX;
extern int g_cursorY;
extern char g_keyDx;
extern char g_keyDy;
extern char g_lastDx;
extern char g_lastDy;
extern char g_buttons;
extern signed char g_cVis;
extern int g_4734;
extern char g_click;
extern char g_click2;
extern int g_clickX;
extern int g_clickY;
extern int g_click2X;
extern int g_click2Y;
extern int g_inputLock;
extern int g_4742;
extern int g_4744;
extern int g_savedCursorX;
extern int g_savedCursorY;
extern int g_cursorStepX[3];
extern int g_cursorStepY[3];
extern signed char g_directionSteps[8][2];
extern char g_scanMap[];
extern char g_demoKey;
extern char g_fireLatch;
extern char g_soundOn;
extern char g_speakerOn;
extern char g_lastKey;
extern char g_47d1[3];
extern int g_oldInt9Seg;
extern int g_oldInt9Off;
extern int g_dirHeld[8];
extern int g_joySavedSp;
extern char g_modeBitsA;
extern char g_biosModeA;
extern char g_modeBitsB;
extern char g_biosModeB;
extern int g_firePressed;
extern char g_47f0;
extern char g_gearText[8];
extern struct Item g_parts[];
extern int g_partWear[5];
extern int g_betRaces;
extern int g_dragWins;
extern int g_roadWins;
extern int g_kingDragWon;
extern char g_manifoldFit[][5];
extern char g_carbFit[][3];
extern int g_partPics[][5];
extern int g_boltPics[];
extern int g_transPics[];
extern char *g_cashLabel;
extern char *g_gearLabel;
extern int g_list[];
void id_store_mode_a(char a, char b);
void id_store_mode_b(char a, char b);

#endif
