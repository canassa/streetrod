/**
 * @file SCREEN.H
 * @brief Status messages, money display and picture placement.
 */

#ifndef SCREEN_H
#define SCREEN_H

#include "GAME.H"
#include "STRUCTS.H"

struct PartNode;
struct CarPoolNode;

/** @brief Saved race selection used for wagers and course setup. */
enum RaceType {
    RACE_DRAG = 0, /**< Straight course with a standing start. */
    RACE_ROAD = 1  /**< Winding course with roadside hazards. */
};

/** @brief Player cash, owned inventory, allocation pools and race selection. */
struct DriverState {
    long money;                   /**< Cash in dollars. */
    int unused;                   /**< Reserved word, initialized to ten. */
    struct Car *currentCar;       /**< Selected owned car, or zero. */
    struct Car *otherCars;        /**< Other owned cars, linked from the first. */
    struct CarPoolNode *freeCars; /**< Head of the free car-node pool. */
    int carCount;                 /**< Allocated car nodes, including the selected car. */
    struct PartNode *spareParts;  /**< Head of the uninstalled parts list. */
    struct PartNode *freeParts;   /**< Head of the free part-node pool. */
    int partCount;                /**< Allocated part nodes, including fitted parts. */
    int adIndex;                  /**< Newspaper advertisement resume index. */
    enum RaceType raceType;       /**< Selected course for the next race. */
};

/** @brief Pen position returned by the last text drawing operation. */
struct TextEnd {
    int x; /**< Pixel column immediately after the text. */
    int y; /**< Top pixel row at which the text was drawn. */
};

/** @brief Status-strip text placement and pending erasure state. */
struct StatusText {
    int messageX;     /**< Left edge of the default message area, in pixels. */
    int y;            /**< Top pixel row of status text. */
    int messageWidth; /**< Default centering width in pixels. */
    BOOL needsErase;  /**< Erase the previous text before the next write. */
    int valueX;       /**< Byte-aligned pixel column for money or gear text. */
};

/** @brief Destination and height cap for the next loaded scene picture. */
struct PicturePlacement {
    int x;         /**< Destination left pixel column; reset after drawing. */
    int y;         /**< Destination top row; reset after drawing. */
    int maxHeight; /**< Maximum picture height and optional clear height. */
};

extern struct TextEnd g_penPoint;
extern struct StatusText g_statusLine;

/**
 * @name Screen vertical partition
 * @brief Pixel rows available for scene drawing and the status strip.
 * @{
 */
#define SCREEN_BODY_HEIGHT 190
#define STATUS_BAR_HEIGHT 10
/** @} */

extern struct PicturePlacement g_picturePlacement;
extern struct DriverState g_driver;
extern char g_displaySplitYVal;
extern int g_xMoneyRhsVal;
void sc_status_init(int y);
void sc_status_text(int x, char *s, int w);
void sc_add_money(int n);
void sc_label(char *s);
void sc_show_gear(int i);
void sc_fill_rect(int x, int y, int w, int h, char color);
void sc_msg_box(char *s);
void sc_wait_msg(char *s);
int sc_picture(int a, int b);
int sc_picture_at(int a, int b, int c, int d);
void sc_text(int x, int y, char *s);
void sc_hline(int x, int y, int w);

#endif
