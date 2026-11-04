/* ADS.H - declarations of what ADS.C defines. */

#ifndef ADS_H
#define ADS_H
#include "STRUCTS.H"

extern struct Blt g_adPacket;

extern struct Pages g_pages;
extern struct Bitmap far *g_adPicture;
extern char g_adRedraw;
extern int g_adResume;
extern int g_adIdxEndPageState;
void av_open(int *list, int base);
int av_run(void);
void av_close(void);

#endif
