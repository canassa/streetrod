/* VIDDET.H - declarations of what VIDDET.ASM defines. */

#ifndef VIDDET_H
#define VIDDET_H

extern char g_savedVideoMode;
extern char g_egaMono;
extern char g_savedEquip;
extern char g_egaMemory;
extern char g_egaSwitches;
extern char g_5af3;
void vd_restore(void);
void vd_detect(void);

#endif
