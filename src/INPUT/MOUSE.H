/**
 * @file MOUSE.H
 * @brief Mouse driver detection, buttons and relative motion.
 */

#ifndef MOUSE_H
#define MOUSE_H

int mo_reset(void);
void mo_buttons(void);
void mo_motion(int *dx, int *dy);
void mo_poll(void);

#endif
