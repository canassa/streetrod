/**
 * @file SCREEN.H
 * @brief Status messages, money display and picture placement.
 */

#ifndef SCREEN_H
#define SCREEN_H

#include "GAME.H"

struct Node;
struct Node2;

/** @brief Saved race selection used for wagers and course setup. */
enum RaceType {
    RACE_DRAG = 0, /**< Straight course with a standing start. */
    RACE_ROAD = 1  /**< Winding course with roadside hazards. */
};

struct DriverState {
    long money;
    int unused;
    struct Car *currentCar;
    struct Car *otherCars;
    struct Node2 *freeCars;
    int carCount;
    struct Node *spareParts;
    struct Node *freeParts;
    int partCount;
    int adIndex;
    enum RaceType raceType;
};

struct TextEnd {
    int x;
    int y;
};

struct StatusText {
    int messageX;
    int y;
    int messageWidth;
    int shown;
    int valueX;
};

/* Picture destination and maximum drawing height. */
struct PicturePlacement {
    int x;
    int y;
    int maxHeight;
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
