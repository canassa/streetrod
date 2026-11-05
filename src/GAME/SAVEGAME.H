/* SAVEGAME.H - declarations of what SAVEGAME.C defines. */

#ifndef SAVEGAME_H
#define SAVEGAME_H

enum SaveLoadResult {
    LOAD_OK = 0,
    LOAD_CANCELLED = -1,
    LOAD_ERROR = -666
};

extern int g_demoRacer;
void sg_init_racer(int k, int *list);
int near sg_new(void);
enum SaveLoadResult near sg_load(void);
void near sg_save(void);
int near sg_menu(void);

#endif
