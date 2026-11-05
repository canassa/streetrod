/**
 * @file ADS.H
 * @brief Paged newspaper advertisement lists.
 */

#ifndef ADS_H
#define ADS_H

struct Bitmap;

/* List menu pages (av_open fills them, av_run pages through them): start[k] is the
   first entry of page k, n the page count, cur the current page, list the menu (count, then items). */
struct Pages {
    int start[12];
    int n;
    int cur;
    int *list;
};

extern int g_adIdxEndPageState;
void av_open(int *list, int base);
int av_run(void);
void av_close(void);

#endif
