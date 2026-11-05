/**
 * @file VIDDET.H
 * @brief Video adapter detection and original mode restoration.
 */

#ifndef VIDDET_H
#define VIDDET_H

extern char g_savedVideoMode;
extern char g_egaMono;
extern char g_savedEquip;
extern char g_egaMemory;
extern char g_egaSwitches;
void vd_restore(void);
void vd_detect(void);

#endif
