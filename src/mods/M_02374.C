#include "STRUCTS.H"
#include "M_00D58.H"
#include "M_00000.H"
#include "M_02B9A.H"
#include "M_02DF4.H"
#include "M_0B3CA.H"
#include "M_0F38A.H"
#include "M_10B7C.H"
#include "M_113A2.H"
#include "M_15236.H"
#include "M_163BE.H"
#include "M_16D1A.H"
#include "M_174A6.H"
#include "M_265BA.H"
#include <conio.h>
#include <dos.h>
#include <stdlib.h>

static char g_6c4a;                        /* DS:6C4A */

/* Uninitialized storage: communals, allocated by LINK (DS:6E60..8F02). */
struct State7E8A ar7e8av;
struct State7D2E ao7d2ev;
int ao8228u;
long b_82bc;
int z_8b7c;

/* F_02374 @ 0000:2374 (linear 0x02374, 24 bytes) */


void F_02374(void)
{
    e_78e0.f14 = e_78e0.f16 = e_78e0.f2 = e_78e0.f4 = g_5d6 = 0;
    e_78e0.fbe = e_78e0.fc0;
}

/* F_0238C @ 0000:238C (linear 0x0238C, 2062 bytes) */


#pragma intrinsic(inp, outp, _enable, _disable, abs)

void interrupt far F_0238C(void)
{
    struct Range *sp;
    char flag = 0;
    char s = 0;
    char prev = 0;
    char c = 0;
    char top = 0;
    union Word val;
    int i;
    int off;
    int n;
    unsigned r;

    g_tick++;
    if (g_0608--) {
        outp(0x20, 0x20);
        _enable();
        if (g_47ce) {
            if (g_47ce != g_47cf) {
                outp(0x61, inp(0x61) | 3);
                g_47cf = 1;
            }
            if (g_0608 & 1) {
                if (g_0286 == 0) {
                    r = (unsigned)(((long)e_78e0.fe * e_78e0.fe) >> 10);
                    r += r >> (r + 2) >> 3;
                    val.w = 50000 - ((e_78e0.fe == 5700 ? 10000 : 0) + r);
                    outp(0x42, val.b[0]);
                    outp(0x42, val.b[1]);
                } else if (g_5a0e) {
                    off = ac825et - z_8b7c;
                    r = off < 0 ? -off : off;
                    r = (r >> 1) * r;
                    if (r > 20000)
                        r = 20000;
                    val.w = 60000 - r;
                    outp(0x42, val.b[0]);
                    outp(0x42, val.b[1]);
                }
            } else {
                g_06ea ^= 1;
                if (g_58dc) {
                    if (g_06e4++ < 10)
                        val.w = 0x54b;
                    else
                        val.w = 0x712;
                    outp(0x42, val.b[0]);
                    outp(0x42, val.b[1]);
                    if (g_06e4 > 20)
                        g_06e4 = 0;
                } else if (g_58da || g_5e0e) {
                    if (g_06ea) {
                        val.w = 0x95;
                        outp(0x42, val.b[0]);
                        outp(0x42, val.b[1]);
                    }
                }
            }
        } else if (g_47ce != g_47cf) {
            outp(0x61, inp(0x61) & 0xfc);
            g_47cf = 0;
        }
    } else {
        g_0600++;
        (*w_7aaa)();
        g_0608 = 3;
        if (bw8aca && !g_0286 && --g_0609 <= 0) {
            g_0609 = 3;
            _disable();
            if (!g_06e8) {
                g_06e8 = 1;
                _enable();
                flag = s = prev = c = 0;
                if (g_4740 == 0) {
                    if (n = g_472e) {
                        s = 1;
                        g_472a = n > 0 ? 1 : -1;
                    }
                    if (n = g_472f) {
                        if (n > 0)
                            prev = 1;
                        else
                            flag = 1;
                    }
                    if (g_0610 == 0 && g_471d) {
                        F_02DF4();
                        if ((n = g_4718) < g_4722) {
                            g_6c4a = s = 1;
                            g_472a += -1;
                        } else if (g_4724 < n) {
                            g_6c4a = s = 1;
                            g_472a++;
                        }
                        if ((n = g_471a) < g_4726) {
                            g_6c4a = flag = 1;
                            g_472c += -1;
                        } else if (g_4728 < n) {
                            g_6c4a = prev = 1;
                            g_472c++;
                        }
                    }
                    if (g_0610 == 0 && g_05f2) {
                        F_16DC7();
                        if (g_5e4 > 0) {
                            g_6c4a = 1;
                            g_472c++;
                            prev = 1;
                        }
                        if (n = g_5e2) {
                            g_6c4a = s = 1;
                            g_472a += n > 0 ? 1 : -1;
                        }
                        if (g_5f0)
                            flag = 1;
                    }
                }
                c = g_4732 && !g_06e9;
                g_06e9 = g_4732;
                if (g_47cd) {
                    c = 1;
                    g_47cd = 0;
                }
                if (g_0610 || g_0612) {
                    if (e_78e0.f14 * 4 + e_78e0.fe < 5700) {
                        if (ar7e8av.m7e8a > ao7d2ev.m7d30 ||
                            (!a_7eaap.m7ec0 ? 0x17c : 0x3fc) - 60 < ao7d2ev.m7d30)
                            flag = 1;
                    } else if (e_78e0.f18 < e_78e0.f1c && ao8228u) {
                        c = 1;
                        flag = 1;
                        g_06ac++;
                    }
                    if (e_78e0.fe < 2000 && e_78e0.f18 > 1 && !flag)
                        c = 1;
                    if (e_78e0.f20 && e_78e0.f18 > 0)
                        c = 0;
                }
                if (bw8aca) {
                    b_82bc += e_78e0.f0;
                    e_78e0.fc0++;
                }
                if ((!a_7eaap.m7ec0 ? 0x17c : 0x3fc) < ao7d2ev.m7d30) {
                    prev = 1;
                    flag = s = 0;
                    if (e_78e0.f0 > 15)
                        e_78e0.f0 -= 15;
                }
                if (g_54da > 0) {
                    prev = 1;
                    flag = s = 0;
                    g_54da--;
                }
                if (e_78e0.fe > 5000) {
                    a_7eaap.m7eb0->rack.s[0]->b += !a_7eaap.m7ec0 ? 1 : 1;
                    a_7eaap.m7eb0->slot6->b += !a_7eaap.m7ec0 ? 1 : 1;
                }
                if (!(g_0600 & (!a_7eaap.m7ec0 ? 0xf : 0xf)))
                    a_7eaap.m7eb0->rack.s[0]->b++;
                if ((i = a_7eaap.m7eb0->rack.s[0]->b) >= 9900) {
                    e_78e0.fbc |= 5;
                    F_02374();
                }
                if (flag) {
                    e_78e0.fe += e_78e0.f14;
                    if (e_78e0.f20 && e_78e0.fe > 5690)
                        e_78e0.fe = 5690;
                    if (e_78e0.fe > 5700) {
                        e_78e0.fe = 5700;
                        if (15 - i / 1000 < ++g_06e6) {
                            e_78e0.fbc |= 5;
                            F_02374();
                        }
                    }
                } else if (e_78e0.fe > 1500 ||
                           (e_78e0.f18 > 0 && e_78e0.fe > e_78e0.f16))
                    e_78e0.fe -= e_78e0.f16;
                if (e_78e0.fe < 5700)
                    g_06e6 = 0;
                top = e_78e0.f0 < 5 && !flag && e_78e0.f18 > 0;
                if (c || top ||
                    (e_78e0.f20 && e_78e0.f24 > 1 &&
                     ((flag && e_78e0.fe > e_78e0.f22 && e_78e0.f18 < e_78e0.f1c) ||
                      (!flag && e_78e0.f18 > 1 && !e_78e0.f42)))) {
                    a_7eaap.m7eb0->slot6->b += e_78e0.fe > 5000 ? 15 : 3;
                    if (g_0610 == 0) {
                        i = a_7eaap.m7eb0->slot6->b;
                        if (i >= 9900 ||
                            (e_78e0.f20 == 0 && flag && e_78e0.fe > 5000 &&
                             i / 100 + 100 > random_int(3277))) {
                            e_78e0.fbc |= 0x11;
                            F_02374();
                        }
                    }
                    if ((prev || (!e_78e0.f42 && !flag)) && e_78e0.f18 > 0)
                        e_78e0.f18--;
                    else if ((flag || (e_78e0.f42 > 0 && !prev)) &&
                             e_78e0.f18 < e_78e0.f1c)
                        e_78e0.f18++;
                    if (e_78e0.f20) {
                        if (c)
                            e_78e0.f24 = e_78e0.f24 == 1 ? 2 : 1;
                        if (top || e_78e0.f18 == 0)
                            e_78e0.f24 = 1;
                        if (e_78e0.f24 == 1)
                            e_78e0.f18 = 0;
                    } else if (top)
                        e_78e0.f18 = 0;
                    e_78e0.f1a = 1;
                    e_78e0.f40 = e_78e0.f34[e_78e0.f18];
                    if ((e_78e0.fe = e_78e0.f0 * e_78e0.f40) == 0)
                        e_78e0.fe = 1505;
                }
                i = -1;
                do {
                    sp = &e_78e0.gear[e_78e0.f18].r[++i];
                } while (i < 2 && sp->f2 < e_78e0.fe);
                if (i != e_78e0.f42 || e_78e0.f1a) {
                    e_78e0.f42 = i;
                    e_78e0.f14 = sp->f4;
                    e_78e0.f16 = sp->f6;
                }
                if (prev || (g_5e0e && e_78e0.f0 > 8)) {
                    if ((e_78e0.f0 -= (e_78e0.f0 >> 4) + 2) < 0)
                        e_78e0.f0 = 0;
                    if ((e_78e0.fe = e_78e0.f0 * e_78e0.f40) < 1500)
                        e_78e0.fe = 1500;
                } else if (e_78e0.f18 == 0 && e_78e0.f0) {
                    if (e_78e0.f0)
                        e_78e0.f0--;
                    e_78e0.fe = 30 * e_78e0.f0 + 1500;
                } else if (e_78e0.f40)
                    e_78e0.f0 = e_78e0.fe / e_78e0.f40;
                if (s && abs(e_78e0.f2 + g_472a) > abs(e_78e0.f2)) {
                    if (abs(g_472a) + abs(e_78e0.f4) == abs(e_78e0.f4 + g_472a) &&
                        abs(e_78e0.f2) < abs(e_78e0.f4))
                        e_78e0.f2 = e_78e0.f4;
                    e_78e0.f2 += g_472a << 1;
                    if (abs(e_78e0.f2) > 24)
                        e_78e0.f2 = e_78e0.f2 > 0 ? 24 : -24;
                    g_472a = 0;
                    e_78e0.f4 = e_78e0.f2;
                } else
                    e_78e0.f2 = 0;
                if (abs(e_78e0.f4) > 0)
                    e_78e0.f4 += e_78e0.f4 > 0 ? -((e_78e0.f4 >> 2) + 1)
                                               : ((-e_78e0.f4 >> 2) + 1);
                if (bw8aca && F_284BB()) {
                    e_78e0.fbc |= 0x21;
                    F_02374();
                }
                g_06e8 = 0;
            }
        }
    }
}
