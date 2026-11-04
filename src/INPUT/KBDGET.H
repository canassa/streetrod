/* KBDGET.H - declarations of what KBDGET.ASM defines. */

#ifndef KBDGET_H
#define KBDGET_H

#define KEY_NONE 0
#define ASCII_ENTER 0x0d
#define ASCII_ESCAPE 0x1b
#define ASCII_DELETE 0x7f
#define KEY_FIRE 0x1f

char kg_key(void);
void kg_hook(void);

#endif
