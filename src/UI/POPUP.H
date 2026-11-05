/**
 * @file POPUP.H
 * @brief Saved-background windows and speech bubbles.
 */

#ifndef POPUP_H
#define POPUP_H

struct Bitmap;

/** @brief Speech-bubble picture, position and text placement. */
struct PopupLayout {
    int pictureId; /**< Speech-bubble picture ID. */
    int lineCount; /**< Number of message lines. */
    int left;      /**< Left screen pixel. */
    int top;       /**< Top screen row. */
    int width;     /**< Background capture width in pixels. */
    int height;    /**< Background capture height in rows. */
    int textX;     /**< Text centering offset from the left edge. */
    int textY;     /**< Text offset below the top edge. */
    int unused;    /**< Unused placement word. */
};

struct Bitmap far *pu_grab(int x, int y, int w, int h, struct Bitmap far *src);
void pu_put(struct Bitmap far *bm, int x, int y);
void pu_show(struct Bitmap far *bm, int x, int y);
void pu_open(int n);
void pu_open_at(int n, int x, int y);
void pu_close(void);

#endif
