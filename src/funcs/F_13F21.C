/* F_13F21: pop the saved state stack: close the current entry; when keep is set and an entry remains,
   reopen the one below it, otherwise reset the entry globals to "none" */

#include "GLOBALS.H"
#include "PROTOS.H"

void F_13F21(int keep)
{
    if (g_8b8e) {
        F_01391(0);
        g_8ba6 = 0;
        if (--g_8b8e && keep) {
            --g_8b8e;
            F_13DE6(g_8b86[g_8b8e]);
        } else {
            g_8efe = 0;
            g_8ba6 = 0;
            g_8b90.x1 = g_8b90.y1 = g_8b90.x2 = g_8b90.y2 = g_8baa = -1;
        }
    }
}
