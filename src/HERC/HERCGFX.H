/**
 * @file HERCGFX.H
 * @brief Hercules graphics mode setup.
 */

#ifndef HERCGFX_H
#define HERCGFX_H

extern int g_hercCrtc[10];
extern char g_hercBiosData[30];
void far hg_set_mode(void);

#endif
