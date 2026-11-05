/**
 * @file CGALINE.H
 * @brief CGA line drawing and color dithering tables.
 */

#ifndef CGALINE_H
#define CGALINE_H

extern char g_cgaColorByte[4];
extern char g_cgaColorMap[16];
extern char g_cgaDitherOdd[16];
extern char g_cgaDitherEven[16];
void cl_draw(int x1, int y1, int x2, int y2, int color);
void cl_span(int x1, int x2, int y, int color);
void cl_fill_below(int y1, int color);

#endif
