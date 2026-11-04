#include "STRUCTS.H"
#include "M_113A2.H"
#include "M_00000.H"
#include "M_13682.H"
#include "M_19ED0.H"

/* F_119D6: draw string s at (x, y) on the back buffer, then copy the 8 pixel high strip to the front buffer */


void F_119D6(int x, int y, char *s)
{
    (*a_78aem)(r_vid.back, x, y, s);
    if (r_vid.f16)
        F_1A92A(r_vid.back, r_vid.front, x, y, g824cd - 1, y + 7);
}

/* F_11A24: draw a horizontal run of w pixels at (x, y) on the back buffer and copy it to the front buffer */


void F_11A24(int x, int y, int w)
{
    F_1377F(r_vid.back, x, y, w);
    if (r_vid.f16)
        F_1A92A(r_vid.back, r_vid.front, x, y, x + w - 1, y);
}
