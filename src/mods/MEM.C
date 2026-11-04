#include <dos.h>
#include <malloc.h>
#include "STRUCTS.H"
#include "MEM.H"
#include "MAIN.H"
#include "PC.H"
#include "POOLS.H"
#include "SCREEN.H"
#include "SYS.H"

/* File-local storage (_BSS): names chosen to fit MSC's name-hash order (docs/RECONSTRUCTION.md). */
static unsigned g_dataSeg;                    /* DS:6C60 */

/* mem_data_segment @ 0000:36A8 (linear 0x036A8, 22 bytes) */

int mem_data_segment(void)
{
    struct SREGS segs;

    segread(&segs);
    return segs.ds;
}

/* mem_init @ 0000:36BE (linear 0x036BE, 24 bytes) */

void near mem_init(void)
{
    _amblksiz = 0x800;
    g_dataSeg = mem_data_segment();
    pools_init(1);
}

/* mem_free @ 0000:36D6 (linear 0x036D6, 19 bytes) */

void mem_free(void far *p)
{
    _ffree(p);
}

char far *g_vramBase = EGA_VRAM;   /* DS:4C14 */
unsigned g_vramUsed[4] = {0, 0, 0, 0};   /* DS:4C18 */
int g_titlePics[4] = {3, 1, 4, 0};   /* DS:4C20 */
int g_gamePics[241] = {1072, 1073, 1074, 1021, 1022, 1023, 1025, 1024, 1026, 1027, 1259, 1260, 1264, 1266, 1265, 1028, 1029, 1030, 1031, 1106, 1107, 1108, 1109, 1065, 1066, 1067, 1068, 1182, 1183, 1184, 1185, 1181, 1039, 1038, 1037, 1033, 1040, 1043, 1041, 1042, 1047, 1046, 1045, 1044, 1048, 1050, 1049, 1053, 1052, 1051, 1054, 1055, 1056, 1060, 1059, 1058, 1180, 1061, 1062, 1112, 1111, 1063, 1064, 1113, 1116, 1114, 1115, 1118, 1117, 1119, 1122, 1120, 1121, 1126, 1125, 1123, 1124, 1127, 1130, 1128, 1129, 1131, 1134, 1132, 1133, 1135, 1144, 1136, 1143, 1145, 1148, 1146, 1147, 1149, 1152, 1150, 1151, 1156, 1155, 1153, 1154, 1157, 1160, 1158, 1159, 1164, 1163, 1161, 1162, 1165, 1235, 1167, 1166, 1171, 1170, 1168, 1169, 1172, 1175, 1173, 1174, 1179, 1178, 1176, 1177, 1193, 1192, 1191, 1190, 1189, 1188, 1186, 1187, 1218, 1196, 1194, 1209, 1197, 1198, 1210, 1213, 1195, 1208, 1211, 1203, 1216, 1202, 1199, 1205, 1207, 1206, 1204, 1212, 1214, 1215, 1223, 1234, 1034, 1035, 1036, 1219, 1221, 1220, 1032, 1200, 1001, 1002, 1000, 1005, 1014, 1006, 1007, 1008, 1009, 1010, 1012, 1011, 1267, 1227, 1228, 1229, 1230, 1231, 1232, 1110, 1263, 1069, 1222, 1070, 1071, 1075, 1076, 1077, 1078, 1081, 1082, 1083, 1104, 1079, 1080, 1084, 1085, 1086, 1099, 1100, 1101, 1102, 1105, 1103, 1089, 1088, 1087, 1095, 1094, 1015, 1016, 1017, 1018, 1019, 1020, 1090, 1096, 1097, 1098, 1137, 1138, 1139, 1224, 1226, 1233, 1237, 1236, 1238, 1093, 1201, 1244, 1140, 1141, 1142, 1225, 0};   /* DS:4C28 */

/* mem_out_of_memory @ 0000:36E9 (linear 0x036E9, 32 bytes) */

void mem_out_of_memory(void)
{
    char *msg;

    msg = "Not enough memory to run Street Rod !";
    screen_wait_msg(msg);
    sys_exit();
}

/* mem_alloc @ 0000:3709 (linear 0x03709, 98 bytes) */

char far *mem_alloc(unsigned n, int fatal)
{
    char far *p;

    if (p = _fmalloc(n)) {
        if (FP_SEG(p) == g_dataSeg ||
            (r_vid.mode == VID_TANDY && (unsigned long)(p + n) >= 0x80000000UL))
            _ffree(p);
        else
            return p;
    }
    if (fatal)
        mem_out_of_memory();
    else
        return 0;
}

/* mem_new_bitmap @ 0000:376B (linear 0x0376B, 277 bytes) */

struct Bitmap far *mem_new_bitmap(int w, int h, int flags, int c, struct Bitmap far *mem)
{
    int pitch;
    int extra6;
    int hdrl;
    unsigned size;
    unsigned planesize;
    struct Bitmap far *bm;

    pitch = ((w - 1) >> r_vid.shift) + 1;
    size = pitch * h;
    planesize = size * r_vid.planes;
    hdrl = 0x30;
    extra6 = 0;
    if (flags)
        extra6 = size;
    if (!(flags & 2))
        hdrl += planesize;
    if (mem == 0) {
        bm = (struct Bitmap far *)mem_alloc(hdrl + extra6, 0);
        if (bm == 0)
            return 0;
        bm->bits.p = (char far *)bm + 0x30;
    } else {
        bm = mem;
    }
    if (flags & 2) {
        if (bm->bits.p) {
            bm->alt.p = bm->bits.p;
            bm->bits.p = 0;
        } else {
            bm->alt.p = (char far *)bm + 0x30;
        }
    } else {
        bm->alt.p = flags ? bm->bits.p + planesize : 0;
    }
    bm->planesize = planesize;
    bm->width = w;
    bm->height = h;
    bm->pitch = pitch;
    bm->flags = c;
    return bm;
}

/* mem_wrap_bitmap @ 0000:3880 (linear 0x03880, 132 bytes) */

struct Bitmap far *mem_wrap_bitmap(char far *bits, char far *alt, int w, int h, int flags)
{
    int pitch;
    int size;
    struct Bitmap far *bm;

    pitch = ((w - 1) >> r_vid.shift) + 1;
    size = pitch * h * r_vid.planes;
    bm = (struct Bitmap far *)mem_alloc(0x30, 0);
    if (bm == 0)
        return 0;
    bm->bits.p = bits;
    bm->alt.p = alt;
    bm->planesize = size;
    bm->width = w;
    bm->height = h;
    bm->pitch = pitch;
    bm->flags = flags;
    return bm;
}

/* mem_set_bitmap @ 0000:3904 (linear 0x03904, 99 bytes) */

void mem_set_bitmap(struct Bitmap far *bm, char far *bits, char far *alt, int w, int h, int flags)
{
    int pitch;
    int size;

    pitch = ((w - 1) >> r_vid.shift) + 1;
    size = pitch * h * r_vid.planes;
    bm->bits.p = bits;
    bm->alt.p = alt;
    bm->planesize = size;
    bm->width = w;
    bm->height = h;
    bm->pitch = pitch;
    bm->flags = flags;
}

/* mem_vram_alloc @ 0000:3967 (linear 0x03967, 88 bytes) */

char far *mem_vram_alloc(unsigned size, char *pool)
{
    char far *p;
    int i;

    for (i = 0; i < 4; i++) {
        if (g_vramUsed[i] + size < 0x7d00) {
            p = g_vramBase + g_vramUsed[i] + 0x7d00;
            g_vramUsed[i] += size;
            *pool = i;
            return p;
        }
    }
    *pool = -1;
    return 0;
}
