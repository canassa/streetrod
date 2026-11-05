/**
 * @file CRTC.H
 * @brief Display synchronization and split-screen control.
 */

#ifndef CRTC_H
#define CRTC_H

void ct_wait_vsync(void);
void ct_set_split(int line);

#endif
