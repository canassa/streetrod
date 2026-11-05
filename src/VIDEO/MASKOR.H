/**
 * @file MASKOR.H
 * @brief Masked pixel overlay transfers.
 */

#ifndef MASKOR_H
#define MASKOR_H

void mr_blit(char far *dst, unsigned sseg, unsigned soff, unsigned moff, int a, int w, int h, int c);

#endif
