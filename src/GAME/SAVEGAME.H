/* SAVEGAME.H - declarations of what SAVEGAME.C defines. */

#ifndef SAVEGAME_H
#define SAVEGAME_H

enum SaveLoadResult {
    LOAD_OK = 0,
    LOAD_CANCELLED = -1,
    LOAD_ERROR = -666
};

extern char g_noNameMsg[];
extern char g_saveSlots[];
extern int g_slotNameOfs[];
extern char *g_noNameMsgPtr;
extern int g_demoRacer;
extern char g_buyCarMsg[];
void sg_init_racer(int k, int *list);
int near sg_new(void);
enum SaveLoadResult near sg_load(void);
void near sg_save(void);
int near sg_menu(void);

#endif
