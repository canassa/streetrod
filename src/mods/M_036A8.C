#include <dos.h>
#include "STRUCTS.H"
#include "M_036A8.H"
#include "M_00000.H"
#include "M_00D58.H"
#include "M_113A2.H"
#include <malloc.h>

static unsigned g_6c60;                    /* DS:6C60 */

static void near F_03614(int reset);

/* data_segment @ 0000:36A8 (linear 0x036A8, 22 bytes) */


int data_segment(void)
{
    struct SREGS segs;

    segread(&segs);
    return segs.ds;
}

static void near F_036BE(void)
{
    _amblksiz = 0x800;
    g_6c60 = data_segment();
    F_03614(1);
}

void F_036D6(void far *p)
{
    _ffree(p);
}

char far *g_4c14 = (char far *)0xA0000000L;   /* DS:4C14 */
unsigned g_4c18[4] = {0, 0, 0, 0};   /* DS:4C18 */
int g_4c20[4] = {3, 1, 4, 0};   /* DS:4C20 */
int g_4c28[241] = {1072, 1073, 1074, 1021, 1022, 1023, 1025, 1024, 1026, 1027, 1259, 1260, 1264, 1266, 1265, 1028, 1029, 1030, 1031, 1106, 1107, 1108, 1109, 1065, 1066, 1067, 1068, 1182, 1183, 1184, 1185, 1181, 1039, 1038, 1037, 1033, 1040, 1043, 1041, 1042, 1047, 1046, 1045, 1044, 1048, 1050, 1049, 1053, 1052, 1051, 1054, 1055, 1056, 1060, 1059, 1058, 1180, 1061, 1062, 1112, 1111, 1063, 1064, 1113, 1116, 1114, 1115, 1118, 1117, 1119, 1122, 1120, 1121, 1126, 1125, 1123, 1124, 1127, 1130, 1128, 1129, 1131, 1134, 1132, 1133, 1135, 1144, 1136, 1143, 1145, 1148, 1146, 1147, 1149, 1152, 1150, 1151, 1156, 1155, 1153, 1154, 1157, 1160, 1158, 1159, 1164, 1163, 1161, 1162, 1165, 1235, 1167, 1166, 1171, 1170, 1168, 1169, 1172, 1175, 1173, 1174, 1179, 1178, 1176, 1177, 1193, 1192, 1191, 1190, 1189, 1188, 1186, 1187, 1218, 1196, 1194, 1209, 1197, 1198, 1210, 1213, 1195, 1208, 1211, 1203, 1216, 1202, 1199, 1205, 1207, 1206, 1204, 1212, 1214, 1215, 1223, 1234, 1034, 1035, 1036, 1219, 1221, 1220, 1032, 1200, 1001, 1002, 1000, 1005, 1014, 1006, 1007, 1008, 1009, 1010, 1012, 1011, 1267, 1227, 1228, 1229, 1230, 1231, 1232, 1110, 1263, 1069, 1222, 1070, 1071, 1075, 1076, 1077, 1078, 1081, 1082, 1083, 1104, 1079, 1080, 1084, 1085, 1086, 1099, 1100, 1101, 1102, 1105, 1103, 1089, 1088, 1087, 1095, 1094, 1015, 1016, 1017, 1018, 1019, 1020, 1090, 1096, 1097, 1098, 1137, 1138, 1139, 1224, 1226, 1233, 1237, 1236, 1238, 1093, 1201, 1244, 1140, 1141, 1142, 1225, 0};   /* DS:4C28 */

/* out_of_memory @ 0000:36E9 (linear 0x036E9, 32 bytes) */


void out_of_memory(void)
{
    char *msg;

    msg = "Not enough memory to run Street Rod !";
    F_1174E(msg);
    F_00FEA();
}

char far *F_03709(unsigned n, int fatal)
{
    char far *p;

    if (p = _fmalloc(n)) {
        if (FP_SEG(p) == g_6c60 ||
            (r_vid.mode == -6 && (unsigned long)(p + n) >= 0x80000000UL))
            _ffree(p);
        else
            return p;
    }
    if (fatal)
        out_of_memory();
    else
        return 0;
}

struct Bitmap far *F_0376B(int w, int h, int flags, int c, struct Bitmap far *mem)
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
        bm = (struct Bitmap far *)F_03709(hdrl + extra6, 0);
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

struct Bitmap far *F_03880(char far *bits, char far *alt, int w, int h, int flags)
{
    int pitch;
    int size;
    struct Bitmap far *bm;

    pitch = ((w - 1) >> r_vid.shift) + 1;
    size = pitch * h * r_vid.planes;
    bm = (struct Bitmap far *)F_03709(0x30, 0);
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

void F_03904(struct Bitmap far *bm, char far *bits, char far *alt, int w, int h, int flags)
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
