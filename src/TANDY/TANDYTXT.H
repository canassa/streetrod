/* TANDYTXT.H - declarations of what TANDYTXT.ASM defines. */

#ifndef TANDYTXT_H
#define TANDYTXT_H

int tt_render(int x, int glyph, char *buf, int c1, int c2);
void tt_copy(char *buf, unsigned seg, unsigned off, int y, int h, int m, int ph, int n);

#endif
