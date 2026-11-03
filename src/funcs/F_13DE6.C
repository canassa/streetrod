/* F_13DE6: push hot spot id onto the saved state stack (at most four deep) and draw it: the
   current entry globals are reset, F_139DE draws the linked spot, and the touched area is copied
   from the back buffer to the front when the screen shadow is on */

#include "GLOBALS.H"
#include "PROTOS.H"

void F_13DE6(int id)
{
    struct Hot *p;
    struct Colors saved;

    if (g_8b8e < 4) {
        F_120FE(-3);
        g_8b86[g_8b8e++] = g_8efe = id;
        g_8b90.x1 = g_8b90.x2 = g_8b90.y1 = g_8b90.y2 = -1;
        g_8bac = (p = &g_09f4[g_2bad[id]])->f4;
        g_8ba6 = (p + 1)->f0 > 9 ? p + 1 : 0;
        g_8b9c = g_8b9a = g_8ba2 = 0;
        g_8ba4 = 0;
        g_8b98 = p->f0 == 3;
        saved = g_colors;
        g_07a8 = -1;
        g_6c56 = g_6c58 = 1000;
        g_6c5a = g_6c5c = -1;
        if (p->f10)
            F_139DE(&g_09f4[p->f10]);
        g_07a8 = 0;
        g_colors = saved;
        F_120FE(-1);
        if (g_vid.f16 && g_6c5a >= 0)
            F_1A92A(g_vid.back, g_vid.front, g_6c56 - 3, g_6c58 - 3, g_6c5a + 6, g_6c5c + 6);
        F_012DE(id);
        F_00D78();
    }
}
