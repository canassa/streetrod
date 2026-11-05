/**
 * @file SAVEGAME.H
 * @brief New-game initialization and saved-game menus.
 */

#ifndef SAVEGAME_H
#define SAVEGAME_H

/**
 * @name New-game cash
 * @brief Initial dollar balances for player and demo games.
 * @{
 */
#define STARTING_MONEY 750L
#define DEMO_STARTING_MONEY 10000L
/** @} */

/** @brief Saved-game selection and disk-read outcome. */
enum SaveLoadResult {
    LOAD_OK = 0,         /**< Selected game loaded and its clock rebased. */
    LOAD_CANCELLED = -1, /**< Selection dismissed or no saved games exist. */
    /** Selected game could not be read; the caller may retry. */
    LOAD_ERROR = -666
};

extern int g_demoRacer;
void sg_init_racer(int k, int *list);
int near sg_new(void);
enum SaveLoadResult near sg_load(void);
void near sg_save(void);
int near sg_menu(void);

#endif
