#include "GLOBALS.H"
#include "PROTOS.H"
#include <conio.h>
#include <dos.h>
#include <stdlib.h>
#include "STRUCTS.H"

/* F_02374 @ 0000:2374 (linear 0x02374, 24 bytes) */


void F_02374(void)
{
    g_78e0.f14 = g_78e0.f16 = g_78e0.f2 = g_78e0.f4 = g_5d6 = 0;
    g_78e0.fbe = g_78e0.fc0;
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
                    r = (unsigned)(((long)g_78e0.fe * g_78e0.fe) >> 10);
                    r += r >> (r + 2) >> 3;
                    val.w = 50000 - ((g_78e0.fe == 5700 ? 10000 : 0) + r);
                    outp(0x42, val.b[0]);
                    outp(0x42, val.b[1]);
                } else if (g_5a0e) {
                    off = g_825e - g_8b7c;
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
        (*g_7aaa)();
        g_0608 = 3;
        if (g_8aca && !g_0286 && --g_0609 <= 0) {
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
                    if (g_78e0.f14 * 4 + g_78e0.fe < 5700) {
                        if (g_7e8a > g_7d30 ||
                            (!g_7eaa.m7ec0 ? 0x17c : 0x3fc) - 60 < g_7d30)
                            flag = 1;
                    } else if (g_78e0.f18 < g_78e0.f1c && g_8228) {
                        c = 1;
                        flag = 1;
                        g_06ac++;
                    }
                    if (g_78e0.fe < 2000 && g_78e0.f18 > 1 && !flag)
                        c = 1;
                    if (g_78e0.f20 && g_78e0.f18 > 0)
                        c = 0;
                }
                if (g_8aca) {
                    g_82bc += g_78e0.f0;
                    g_78e0.fc0++;
                }
                if ((!g_7eaa.m7ec0 ? 0x17c : 0x3fc) < g_7d30) {
                    prev = 1;
                    flag = s = 0;
                    if (g_78e0.f0 > 15)
                        g_78e0.f0 -= 15;
                }
                if (g_54da > 0) {
                    prev = 1;
                    flag = s = 0;
                    g_54da--;
                }
                if (g_78e0.fe > 5000) {
                    g_7eaa.m7eb0->rack.s[0]->b += !g_7eaa.m7ec0 ? 1 : 1;
                    g_7eaa.m7eb0->slot6->b += !g_7eaa.m7ec0 ? 1 : 1;
                }
                if (!(g_0600 & (!g_7eaa.m7ec0 ? 0xf : 0xf)))
                    g_7eaa.m7eb0->rack.s[0]->b++;
                if ((i = g_7eaa.m7eb0->rack.s[0]->b) >= 9900) {
                    g_78e0.fbc |= 5;
                    F_02374();
                }
                if (flag) {
                    g_78e0.fe += g_78e0.f14;
                    if (g_78e0.f20 && g_78e0.fe > 5690)
                        g_78e0.fe = 5690;
                    if (g_78e0.fe > 5700) {
                        g_78e0.fe = 5700;
                        if (15 - i / 1000 < ++g_06e6) {
                            g_78e0.fbc |= 5;
                            F_02374();
                        }
                    }
                } else if (g_78e0.fe > 1500 ||
                           (g_78e0.f18 > 0 && g_78e0.fe > g_78e0.f16))
                    g_78e0.fe -= g_78e0.f16;
                if (g_78e0.fe < 5700)
                    g_06e6 = 0;
                top = g_78e0.f0 < 5 && !flag && g_78e0.f18 > 0;
                if (c || top ||
                    (g_78e0.f20 && g_78e0.f24 > 1 &&
                     ((flag && g_78e0.fe > g_78e0.f22 && g_78e0.f18 < g_78e0.f1c) ||
                      (!flag && g_78e0.f18 > 1 && !g_78e0.f42)))) {
                    g_7eaa.m7eb0->slot6->b += g_78e0.fe > 5000 ? 15 : 3;
                    if (g_0610 == 0) {
                        i = g_7eaa.m7eb0->slot6->b;
                        if (i >= 9900 ||
                            (g_78e0.f20 == 0 && flag && g_78e0.fe > 5000 &&
                             i / 100 + 100 > random_int(3277))) {
                            g_78e0.fbc |= 0x11;
                            F_02374();
                        }
                    }
                    if ((prev || (!g_78e0.f42 && !flag)) && g_78e0.f18 > 0)
                        g_78e0.f18--;
                    else if ((flag || (g_78e0.f42 > 0 && !prev)) &&
                             g_78e0.f18 < g_78e0.f1c)
                        g_78e0.f18++;
                    if (g_78e0.f20) {
                        if (c)
                            g_78e0.f24 = g_78e0.f24 == 1 ? 2 : 1;
                        if (top || g_78e0.f18 == 0)
                            g_78e0.f24 = 1;
                        if (g_78e0.f24 == 1)
                            g_78e0.f18 = 0;
                    } else if (top)
                        g_78e0.f18 = 0;
                    g_78e0.f1a = 1;
                    g_78e0.f40 = g_78e0.f34[g_78e0.f18];
                    if ((g_78e0.fe = g_78e0.f0 * g_78e0.f40) == 0)
                        g_78e0.fe = 1505;
                }
                i = -1;
                do {
                    sp = &g_78e0.gear[g_78e0.f18].r[++i];
                } while (i < 2 && sp->f2 < g_78e0.fe);
                if (i != g_78e0.f42 || g_78e0.f1a) {
                    g_78e0.f42 = i;
                    g_78e0.f14 = sp->f4;
                    g_78e0.f16 = sp->f6;
                }
                if (prev || (g_5e0e && g_78e0.f0 > 8)) {
                    if ((g_78e0.f0 -= (g_78e0.f0 >> 4) + 2) < 0)
                        g_78e0.f0 = 0;
                    if ((g_78e0.fe = g_78e0.f0 * g_78e0.f40) < 1500)
                        g_78e0.fe = 1500;
                } else if (g_78e0.f18 == 0 && g_78e0.f0) {
                    if (g_78e0.f0)
                        g_78e0.f0--;
                    g_78e0.fe = 30 * g_78e0.f0 + 1500;
                } else if (g_78e0.f40)
                    g_78e0.f0 = g_78e0.fe / g_78e0.f40;
                if (s && abs(g_78e0.f2 + g_472a) > abs(g_78e0.f2)) {
                    if (abs(g_472a) + abs(g_78e0.f4) == abs(g_78e0.f4 + g_472a) &&
                        abs(g_78e0.f2) < abs(g_78e0.f4))
                        g_78e0.f2 = g_78e0.f4;
                    g_78e0.f2 += g_472a << 1;
                    if (abs(g_78e0.f2) > 24)
                        g_78e0.f2 = g_78e0.f2 > 0 ? 24 : -24;
                    g_472a = 0;
                    g_78e0.f4 = g_78e0.f2;
                } else
                    g_78e0.f2 = 0;
                if (abs(g_78e0.f4) > 0)
                    g_78e0.f4 += g_78e0.f4 > 0 ? -((g_78e0.f4 >> 2) + 1)
                                               : ((-g_78e0.f4 >> 2) + 1);
                if (g_8aca && F_284BB()) {
                    g_78e0.fbc |= 0x21;
                    F_02374();
                }
                g_06e8 = 0;
            }
        }
    }
}
