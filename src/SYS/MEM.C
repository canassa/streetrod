#include <dos.h>
#include <malloc.h>
#include "MEM.H"
#include "BITMAP.H"
#include "DISK.H"
#include "MAIN.H"
#include "PC.H"
#include "POOLS.H"
#include "SCREEN.H"
#include "SYS.H"

static unsigned g_dataSeg;


int me_data_segment(void)
{
    struct SREGS segs;

    segread(&segs);
    return segs.ds;
}


void near me_init(void)
{
    _amblksiz = 0x800;
    g_dataSeg = me_data_segment();
    pl_init(1);
}


void me_free(void far *p)
{
    _ffree(p);
}

static char far *g_vramBase = EGA_VRAM;
static unsigned g_vramUsed[4] = {0, 0, 0, 0};
int g_titlePics[4] = {PIC_PUBLISHER_LOGO, PIC_TITLE, PIC_CREDITS_LICENSE_PLATE, 0};
int g_gamePics[241] = {PIC_SPEECH_BUBBLE_LEFT,
                       PIC_SPEECH_BUBBLE_RIGHT,
                       PIC_SPEECH_BUBBLE_TWO_LINES,
                       PIC_NEWSPAPER_MASTHEAD,
                       PIC_NEWS_DODGERS_TIDAL_WAVE,
                       PIC_NEWS_DRAG_RACING_CRACKDOWN,
                       PIC_NEWS_OSCARS_ATLAS_ROCKET,
                       PIC_NEWS_CARS_FROM_JAPAN,
                       PIC_NEWS_VIET_NAM,
                       PIC_NEWSPAPER_CLASSIFIEDS,
                       PIC_CRASHED_WINDSHIELD,
                       PIC_CRASH_WINDOW_CORNER,
                       PIC_CRASH_BLOOD_LEFT,
                       PIC_CRASH_BLOOD_CENTER,
                       PIC_CRASH_BLOOD_RIGHT,
                       PIC_TIRE_BRAND_NAME_1,
                       PIC_TIRE_BRAND_NAME_2,
                       PIC_TIRE_BRAND_NAME_3,
                       PIC_TIRE_BRAND_NAME_4,
                       PIC_TIRE_SLICK_1,
                       PIC_TIRE_SLICK_2,
                       PIC_TIRE_SLICK_3,
                       PIC_TIRE_SLICK_4,
                       PIC_TIRE_STANDARD_1,
                       PIC_TIRE_STANDARD_2,
                       PIC_TIRE_STANDARD_3,
                       PIC_TIRE_STANDARD_4,
                       PIC_ENGINE_OVERLAY_CHEVY_38,
                       PIC_ENGINE_OVERLAY_CORVETTE_56,
                       PIC_ENGINE_OVERLAY_CHEVY_40,
                       PIC_ENGINE_TOP_FORD_32_DEUCE,
                       PIC_CAR_SHADOW,
                       PIC_CHEVY_40_COUPE_BODY,
                       PIC_CHEVY_40_COUPE_ROOF,
                       PIC_CHEVY_40_COUPE_FRONT_BUMPER,
                       PIC_CHEVY_40_COUPE_REAR_BUMPER,
                       PIC_CHEVY_40_ROADSTER_BODY,
                       PIC_CHEVY_40_ROADSTER_ROOF,
                       PIC_CHEVY_40_ROADSTER_FRONT,
                       PIC_CHEVY_40_ROADSTER_REAR,
                       PIC_CHEVY_38_MASTER_BODY,
                       PIC_CHEVY_38_MASTER_ROOF,
                       PIC_CHEVY_38_MASTER_FRONT,
                       PIC_CHEVY_38_MASTER_REAR_BUMPER,
                       PIC_CORVETTE_56_BODY,
                       PIC_CORVETTE_56_FRONT_BUMPER,
                       PIC_CORVETTE_56_REAR_BUMPER,
                       PIC_CORVETTE_61_BODY,
                       PIC_CORVETTE_61_FRONT_BUMPER,
                       PIC_CORVETTE_61_REAR_BUMPER,
                       PIC_CORVETTE_63_BODY,
                       PIC_CORVETTE_63_FRONT_BUMPER,
                       PIC_CORVETTE_63_REAR_BUMPER,
                       PIC_DODGE_55_LANCER_BODY,
                       PIC_DODGE_55_LANCER_ROOF,
                       PIC_DODGE_55_LANCER_FRONT,
                       PIC_DODGE_55_LANCER_REAR_BUMPER,
                       PIC_FORD_32_DEUCE_BODY,
                       PIC_FORD_32_DEUCE_ROOF,
                       PIC_DODGE_62_POLARA_BODY,
                       PIC_DODGE_62_POLARA_ROOF,
                       PIC_DODGE_62_POLARA_FRONT,
                       PIC_DODGE_62_POLARA_REAR_BUMPER,
                       PIC_PLYMOUTH_62_SAVOY_BODY,
                       PIC_PLYMOUTH_62_SAVOY_ROOF,
                       PIC_PLYMOUTH_62_SAVOY_FRONT,
                       PIC_PLYMOUTH_62_SAVOY_REAR,
                       PIC_FORD_32_SEDAN_BODY,
                       PIC_FORD_32_SEDAN_ROOF,
                       PIC_FORD_40_DELUXE_BODY,
                       PIC_FORD_40_DELUXE_ROOF,
                       PIC_FORD_40_DELUXE_FRONT_BUMPER,
                       PIC_FORD_40_DELUXE_REAR_BUMPER,
                       PIC_FORD_55_FAIRLANE_BODY,
                       PIC_FORD_55_FAIRLANE_ROOF,
                       PIC_FORD_55_FAIRLANE_FRONT,
                       PIC_FORD_55_FAIRLANE_REAR,
                       PIC_FORD_57_FAIRLANE_BODY,
                       PIC_FORD_57_FAIRLANE_ROOF,
                       PIC_FORD_57_FAIRLANE_FRONT,
                       PIC_FORD_57_FAIRLANE_REAR,
                       PIC_MERCURY_54_MONTEREY_BODY,
                       PIC_MERCURY_54_MONTEREY_ROOF,
                       PIC_MERCURY_54_MONTEREY_FRONT,
                       PIC_MERCURY_54_MONTEREY_REAR,
                       PIC_MERCURY_56_CUSTOM_BODY,
                       PIC_MERCURY_56_CUSTOM_ROOF,
                       PIC_MERCURY_56_CUSTOM_FRONT,
                       PIC_MERCURY_56_CUSTOM_REAR,
                       PIC_CHEVY_49_STYLELINE_BODY,
                       PIC_CHEVY_49_STYLELINE_ROOF,
                       PIC_CHEVY_49_STYLELINE_FRONT,
                       PIC_CHEVY_49_STYLELINE_REAR,
                       PIC_CHEVY_58_IMPALA_BODY,
                       PIC_CHEVY_58_IMPALA_ROOF,
                       PIC_CHEVY_58_IMPALA_FRONT,
                       PIC_CHEVY_58_IMPALA_REAR_BUMPER,
                       PIC_OLDS_49_88_BODY,
                       PIC_OLDS_49_88_ROOF,
                       PIC_OLDS_49_88_FRONT_BUMPER,
                       PIC_OLDS_49_88_REAR_BUMPER,
                       PIC_OLDS_52_88_BODY,
                       PIC_OLDS_52_88_ROOF,
                       PIC_OLDS_52_88_FRONT_BUMPER,
                       PIC_OLDS_52_88_REAR_BUMPER,
                       PIC_PONTIAC_50_STREAK_BODY,
                       PIC_PONTIAC_50_STREAK_ROOF,
                       PIC_PONTIAC_50_STREAK_FRONT,
                       PIC_PONTIAC_50_STREAK_REAR,
                       PIC_TBIRD_57_BODY,
                       PIC_TBIRD_57_ROOF,
                       PIC_TBIRD_57_FRONT_BUMPER,
                       PIC_TBIRD_57_REAR_BUMPER,
                       PIC_VALIANT_61_V200_BODY,
                       PIC_VALIANT_61_V200_ROOF,
                       PIC_VALIANT_61_V200_FRONT,
                       PIC_VALIANT_61_V200_REAR_BUMPER,
                       PIC_VALIANT_63_V100_BODY,
                       PIC_VALIANT_63_V100_ROOF,
                       PIC_VALIANT_63_V100_FRONT,
                       PIC_VALIANT_63_V100_REAR_BUMPER,
                       PIC_CHEVY_55_BEL_AIR_BODY,
                       PIC_CHEVY_55_BEL_AIR_ROOF,
                       PIC_CHEVY_55_BEL_AIR_FRONT,
                       PIC_CHEVY_55_BEL_AIR_REAR,
                       PIC_STICKER_HOT_LIPS,
                       PIC_STICKER_RINGER,
                       PIC_STICKER_LOST_MARBLES,
                       PIC_STICKER_HOT_ROD,
                       PIC_STICKER_DEAD_END_KIDS,
                       PIC_STICKER_LOVE_MACHINE,
                       PIC_STICKER_ROAD_RUNNER,
                       PIC_STICKER_TWO_NOT_ONE,
                       PIC_STICKER_WIPE,
                       PIC_PORTRAIT_MIKE,
                       PIC_PORTRAIT_RALPH,
                       PIC_PORTRAIT_KIRK,
                       PIC_PORTRAIT_BIFF,
                       PIC_PORTRAIT_BUTCH,
                       PIC_PORTRAIT_WALLY,
                       PIC_PORTRAIT_CHIP,
                       PIC_PORTRAIT_EDDIE,
                       PIC_PORTRAIT_ERNIE,
                       PIC_PORTRAIT_DICK,
                       PIC_PORTRAIT_STEVE,
                       PIC_PORTRAIT_JOHN,
                       PIC_PORTRAIT_JIM,
                       PIC_PORTRAIT_CHARLIE,
                       PIC_PORTRAIT_GEORGE,
                       PIC_PORTRAIT_CLYDE,
                       PIC_PORTRAIT_TOM,
                       PIC_PORTRAIT_ERICA,
                       PIC_PORTRAIT_SPIKE,
                       PIC_PORTRAIT_BILL,
                       PIC_PORTRAIT_VINNIE,
                       PIC_ROLLING_TIRE,
                       PIC_TIRE_SHADOW,
                       PIC_JACK_HANDLE_UP,
                       PIC_JACK_COLUMN,
                       PIC_JACK_HANDLE_DOWN,
                       PIC_CALENDAR_JUNE_JULY,
                       PIC_CALENDAR_AUGUST_SEPTEMBER,
                       PIC_CALENDAR_DAY_OUTLINE,
                       PIC_GARAGE,
                       PIC_STICKER_MENU,
                       PIC_STEERING_WHEEL_CENTER,
                       PIC_STEERING_WHEEL_LEFT,
                       PIC_STEERING_WHEEL_RIGHT,
                       PIC_DASHBOARD_TOP,
                       PIC_COCKPIT,
                       PIC_WINDSHIELD_STRUT,
                       PIC_HOOD_EDGE,
                       PIC_CITY_SKYLINE,
                       PIC_REARVIEW_MIRROR,
                       PIC_SEDAN_PASSING,
                       PIC_SEDAN_REAR_ANGLED,
                       PIC_SEDAN_REAR,
                       PIC_COUNTRY_SKYLINE,
                       PIC_SHIFTER_THREE_SPEED_GATE,
                       PIC_SHIFTER_FOUR_SPEED_GATE,
                       PIC_SHIFTER_AUTOMATIC_GATE,
                       PIC_SHIFTER_THREE_SPEED_KNOB,
                       PIC_SHIFTER_FOUR_SPEED_KNOB,
                       PIC_SHIFTER_AUTOMATIC_KNOB,
                       PIC_SPEEDING_TICKET,
                       PIC_TUNING_HAND,
                       PIC_ENGINE_BAY,
                       PIC_TUNING_BUTTONS,
                       PIC_TRANSMISSION_DIALOG,
                       PIC_TUNING_DIALOG,
                       PIC_FUEL_PUMP_DIGITS,
                       PIC_ENGINE_V6,
                       PIC_ENGINE_V8_SMALL,
                       PIC_ENGINE_V8_LARGE,
                       PIC_MANIFOLD_TWO_BARREL_V8,
                       PIC_MANIFOLD_FOUR_BARREL,
                       PIC_MANIFOLD_DUAL_QUAD,
                       PIC_MANIFOLD_RACING,
                       PIC_MANIFOLD_TRI_POWER,
                       PIC_MANIFOLD_TWO_BARREL_V6,
                       PIC_CARB_TWO_BARREL,
                       PIC_CARB_FOUR_BARREL,
                       PIC_CARB_RACING,
                       PIC_TRANS_AUTOMATIC,
                       PIC_TRANS_THREE_SPEED,
                       PIC_TRANS_FOUR_SPEED,
                       PIC_TRANS_RACING,
                       PIC_TRANSMISSION_LINKAGE,
                       PIC_TRANSMISSION_BOLT_TURNS,
                       PIC_BOLT_TURN_3,
                       PIC_BOLT_TURN_2,
                       PIC_BOLT_TURN_1,
                       PIC_IGNITION_WIRES_CONNECTED,
                       PIC_IGNITION_WIRES_DISCONNECTED,
                       PIC_SAVE_LOAD_BOOK,
                       PIC_DRIVERS_LICENSE,
                       PIC_LICENSE_NAME_STRIP,
                       PIC_LOAD_BUTTON,
                       PIC_SAVE_BUTTON,
                       PIC_CANCEL_BUTTON,
                       PIC_GAS_STATION,
                       PIC_GAS_STATION_MASK,
                       PIC_PUMP_WITHOUT_NOZZLE,
                       PIC_PUMP_MASK_WITHOUT_NOZZLE,
                       PIC_FUEL_NOZZLE,
                       PIC_FUEL_CAP,
                       PIC_FUEL_GAUGE,
                       PIC_PUMP_WITH_NOZZLE,
                       PIC_PUMP_MASK_WITH_NOZZLE,
                       PIC_GAS_ATTENDANT,
                       PIC_SIGN_GARAGE,
                       PIC_SIGN_BOBS,
                       PIC_SIGN_GAS_STATION,
                       PIC_BOBS_DRIVE_IN,
                       PIC_KINGS_CORVETTE_63_BODY,
                       PIC_PORTRAIT_THE_KING,
                       PIC_CARHOP_SILHOUETTES,
                       PIC_CARHOP_WALK_LEFT,
                       PIC_CARHOP_WALK_RIGHT,
                       PIC_CARHOP_IDLE,
                       0};


void me_out_of_memory(void)
{
    char *msg;

    msg = "Not enough memory to run Street Rod !";
    sc_wait_msg(msg);
    sy_exit();
}


char far *me_alloc(unsigned n, int fatal)
{
    char far *p;

    if (p = _fmalloc(n)) {
        if (FP_SEG(p) == g_dataSeg || (r_vid.mode == VID_TANDY && (unsigned long)(p + n) >= 0x80000000UL))
            _ffree(p);
        else
            return p;
    }
    if (fatal)
        me_out_of_memory();
    else
        return 0;
}


struct Bitmap far *me_new_bitmap(int w, int h, int flags, int c, struct Bitmap far *mem)
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
    hdrl = BITMAP_HEADER_BYTES;
    extra6 = 0;
    if (flags)
        extra6 = size;
    if (!(flags & 2))
        hdrl += planesize;
    if (mem == 0) {
        bm = (struct Bitmap far *)me_alloc(hdrl + extra6, 0);
        if (bm == 0)
            return 0;
        bm->bits.p = (char far *)bm + BITMAP_HEADER_BYTES;
    } else {
        bm = mem;
    }
    if (flags & 2) {
        if (bm->bits.p) {
            bm->alt.p = bm->bits.p;
            bm->bits.p = 0;
        } else {
            bm->alt.p = (char far *)bm + BITMAP_HEADER_BYTES;
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


struct Bitmap far *me_wrap_bitmap(char far *bits, char far *alt, int w, int h, int flags)
{
    int pitch;
    int size;
    struct Bitmap far *bm;

    pitch = ((w - 1) >> r_vid.shift) + 1;
    size = pitch * h * r_vid.planes;
    bm = (struct Bitmap far *)me_alloc(BITMAP_HEADER_BYTES, 0);
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


void me_set_bitmap(struct Bitmap far *bm, char far *bits, char far *alt, int w, int h, int flags)
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


char far *me_vram_alloc(unsigned size, char *pool)
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
