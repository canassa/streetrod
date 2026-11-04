/* EGASPAN.H - declarations of what EGASPAN.ASM defines. */

#ifndef EGASPAN_H
#define EGASPAN_H
#include "STRUCTS.H"

void en_begin(void);                               /* 0x26458 */
void en_end(void);                                 /* 0x2646D */
void far en_fill();                                /* 0x26484 */
void far en_fill_below();                          /* 0x26537 */

#endif
