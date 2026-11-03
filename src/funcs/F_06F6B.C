/* F_06F6B @ 0000:6F6B (linear 0x06f6b, 431 bytes) */

#include "GLOBALS.H"
#include "PROTOS.H"

static void near F_06F6B(void)
{
    struct Bitmap far *bm1;
    int x;
    int idx4;
    int y;
    int key;

    if (!g_7eb0)
        F_03AB8(0x0efa);
    else if (g_vid.mode != -2 && g_vid.mode != -6)
        F_03AB8(0x18ad);
    else {
        bm1 = F_14090(14, &x, &y, 2);
        F_13DE6(14);
        idx4 = g_7eb0->f4;
        for (;;) {
            F_112CB(12, g_50c2[idx4][0]);
            while ((key = F_01417(3000)) == 0)
                ;
            switch (key) {
            case 1:
                if (--idx4 < 0)
                    idx4 = 5;
                continue;
            case 2:
                if (++idx4 >= 6)
                    idx4 = 0;
                continue;
            case 3:
                if (g_7eaa < 20L) {
                    F_03AB8(0x10a1);
                    break;
                }
                g_vid.f16 = 1;
                F_13F21(1);
                F_11BD3(bm1, x, y);
                F_19438(1);
                F_11BD3(g_0294, g_8272, g_8274);
                g_7eb0->f24 &= 0xe0ff;
                F_067CF(g_7eb0->f4 = idx4);
                F_112CB(12, 5);
                g_05fc += 0x888L;
                F_11540(-20);
                return;
            case -23:
            default:
                break;
            }
            break;
        }
        F_13F21(1);
        F_112CB(12, 5);
        F_11BD3(bm1, x, y);
        F_19438(1);
    }
}
