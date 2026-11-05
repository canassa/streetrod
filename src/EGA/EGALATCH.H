/**
 * @file EGALATCH.H
 * @brief Rectangle transfers through EGA plane latches.
 */

#ifndef EGALATCH_H
#define EGALATCH_H

void ea_rect(char far *src, char far *dst, int w, int h, int x, int y, int pitch);
void ea_to_page(char far *src, int w, int h, char far *dst, int pitch);

#endif
