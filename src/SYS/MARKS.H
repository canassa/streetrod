/**
 * @file MARKS.H
 * @brief Temporary arena allocation release and stack unwinding.
 */

#ifndef MARKS_H
#define MARKS_H

void ma_clear(void);
void ma_release(void);
void ma_pop_low(int n);
void ma_pop_high(int n);

#endif
