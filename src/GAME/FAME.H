/**
 * @file FAME.H
 * @brief Manual copy protection and hall-of-fame scores.
 */

#ifndef FAME_H
#define FAME_H

/** Maximum number of driver scores retained in the hall of fame. */
#define HALL_FAME_COUNT 10

/**
 * @name Manual question table
 * @brief Select among 51 questions; the first eighteen ask about car icons.
 * @{
 */
#define MANUAL_ENTRY_COUNT 51
#define MANUAL_ICON_COUNT 0x12
/** @} */

#include "VIDEO.H"

/** @brief Decoded color choices for manual copy-protection answers. */
enum ManualColor {
    MANUAL_GREY = 1,          /**< Grey. */
    MANUAL_BROWN = 2,         /**< Brown. */
    MANUAL_RED = 3,           /**< Red. */
    MANUAL_PINK = 4,          /**< Pink. */
    MANUAL_ORANGE = 5,        /**< Orange. */
    MANUAL_YELLOW = 6,        /**< Yellow. */
    MANUAL_DARK_GREEN = 7,    /**< Dark green. */
    MANUAL_LIGHT_GREEN = 8,   /**< Light green. */
    MANUAL_DARK_BLUE = 9,     /**< Dark blue. */
    MANUAL_LIGHT_BLUE = 10,   /**< Light blue. */
    MANUAL_DARK_PURPLE = 11,  /**< Dark purple. */
    MANUAL_LIGHT_PURPLE = 12, /**< Light purple. */
};

/**
 * @brief Manual page and expected color for one copy-protection question.
 *
 * Table position determines whether the question asks about an icon or a key.
 */
struct ManualQuestion {
    signed char page;        /**< Printed manual page number. */
    signed char answerColor; /**< Expected choice, @ref ManualColor. */
};

/**
 * @brief Driver name and elapsed game hours in a hall-of-fame record.
 *
 * Records occupy 23 bytes on disk; saving sets each byte's high bit and loading
 * clears it. Fewer hours rank ahead of more hours.
 */
struct HallEntry {
    char driverName[16];      /**< NUL-terminated name, at most 15 characters. */
    char elapsedHoursText[7]; /**< NUL-terminated decimal hours. */
};

extern int g_protectCycle;
extern int g_nScore;
extern char g_driverName[];
extern struct HallEntry g_hallFameTbl[];
int fa_protect_check(void);
int fa_end_menu(void);
void fa_show(int add);

/** @brief Copy-protection question selected by the manual-table index. */
enum ManualKind {
    /** Ask the key colour for entries after the icon range. */
    MANUAL_CAR_KEY = 0,
    /** Ask the icon colour for the first eighteen entries. */
    MANUAL_CAR_ICON = 1
};

#endif
