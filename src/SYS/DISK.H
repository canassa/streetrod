/**
 * @file DISK.H
 * @brief Game data and picture-library access.
 */

#ifndef DISK_H
#define DISK_H

/** @brief Picture-library IDs; the second library starts at 1000. */
enum PictureId {
    PIC_TITLE = 1,
    PIC_CURSORS = 2,
    PIC_PUBLISHER_LOGO = 3,
    PIC_CREDITS_LICENSE_PLATE = 4,
    PIC_HALL_OF_FAME = 5,
    PIC_ENDING_COUPLE = 6,
    PIC_ENDING_LIPS = 7,
    PIC_COPY_PROTECTION_JUKEBOX = 8,
    PIC_ENDING_MESSAGE = 9,
    PIC_STEERING_WHEEL_RIGHT = 1000,
    PIC_STEERING_WHEEL_CENTER = 1001,
    PIC_STEERING_WHEEL_LEFT = 1002,
    PIC_CORVETTE_PASSING = 1003,
    PIC_CORVETTE_REAR_ANGLED = 1004,
    PIC_DASHBOARD_TOP = 1005,
    PIC_WINDSHIELD_STRUT = 1006,
    PIC_HOOD_EDGE = 1007,
    PIC_CITY_SKYLINE = 1008,
    PIC_REARVIEW_MIRROR = 1009,
    PIC_SEDAN_PASSING = 1010,
    PIC_SEDAN_REAR = 1011,
    PIC_SEDAN_REAR_ANGLED = 1012,
    PIC_START_LIGHTS = 1013,
    PIC_COCKPIT = 1014,
    PIC_SAVE_LOAD_BOOK = 1015,
    PIC_DRIVERS_LICENSE = 1016,
    PIC_LICENSE_NAME_STRIP = 1017,
    PIC_LOAD_BUTTON = 1018,
    PIC_SAVE_BUTTON = 1019,
    PIC_CANCEL_BUTTON = 1020,
    PIC_NEWSPAPER_MASTHEAD = 1021,
    PIC_NEWS_DODGERS_TIDAL_WAVE = 1022,
    PIC_NEWS_DRAG_RACING_CRACKDOWN = 1023,
    PIC_NEWS_CARS_FROM_JAPAN = 1024,
    PIC_NEWS_OSCARS_ATLAS_ROCKET = 1025,
    PIC_NEWS_VIET_NAM = 1026,
    PIC_NEWSPAPER_CLASSIFIEDS = 1027,
    PIC_TIRE_BRAND_NAME_1 = 1028,
    PIC_TIRE_BRAND_NAME_2 = 1029,
    PIC_TIRE_BRAND_NAME_3 = 1030,
    PIC_TIRE_BRAND_NAME_4 = 1031,
    PIC_GARAGE = 1032,
    PIC_CHEVY_40_COUPE_FRONT_BUMPER = 1033,
    PIC_JACK_HANDLE_UP = 1034,
    PIC_JACK_COLUMN = 1035,
    PIC_JACK_HANDLE_DOWN = 1036,
    PIC_CHEVY_40_COUPE_REAR_BUMPER = 1037,
    PIC_CHEVY_40_COUPE_ROOF = 1038,
    PIC_CHEVY_40_COUPE_BODY = 1039,
    PIC_CHEVY_40_ROADSTER_BODY = 1040,
    PIC_CHEVY_40_ROADSTER_REAR = 1041,
    PIC_CHEVY_40_ROADSTER_FRONT = 1042,
    PIC_CHEVY_40_ROADSTER_ROOF = 1043,
    PIC_CHEVY_38_MASTER_FRONT_BUMPER = 1044,
    PIC_CHEVY_38_MASTER_REAR = 1045,
    PIC_CHEVY_38_MASTER_ROOF = 1046,
    PIC_CHEVY_38_MASTER_BODY = 1047,
    PIC_CORVETTE_56_BODY = 1048,
    PIC_CORVETTE_56_FRONT_BUMPER = 1049,
    PIC_CORVETTE_56_REAR_BUMPER = 1050,
    PIC_CORVETTE_61_FRONT_BUMPER = 1051,
    PIC_CORVETTE_61_REAR_BUMPER = 1052,
    PIC_CORVETTE_61_BODY = 1053,
    PIC_CORVETTE_63_BODY = 1054,
    PIC_CORVETTE_63_REAR_BUMPER = 1055,
    PIC_CORVETTE_63_FRONT_BUMPER = 1056,
    PIC_CORVETTE_REAR = 1057,
    PIC_DODGE_55_LANCER_REAR = 1058,
    PIC_DODGE_55_LANCER_ROOF = 1059,
    PIC_DODGE_55_LANCER_BODY = 1060,
    PIC_FORD_32_DEUCE_BODY = 1061,
    PIC_FORD_32_DEUCE_ROOF = 1062,
    PIC_DODGE_62_POLARA_REAR = 1063,
    PIC_DODGE_62_POLARA_FRONT_BUMPER = 1064,
    PIC_TIRE_STANDARD_1 = 1065,
    PIC_TIRE_STANDARD_2 = 1066,
    PIC_TIRE_STANDARD_3 = 1067,
    PIC_TIRE_STANDARD_4 = 1068,
    PIC_ENGINE_BAY = 1069,
    PIC_TRANSMISSION_DIALOG = 1070,
    PIC_TUNING_DIALOG = 1071,
    PIC_SPEECH_BUBBLE_LEFT = 1072,
    PIC_SPEECH_BUBBLE_RIGHT = 1073,
    PIC_SPEECH_BUBBLE_TWO_LINES = 1074,
    PIC_FUEL_PUMP_DIGITS = 1075,
    PIC_ENGINE_V6 = 1076,
    PIC_ENGINE_V8_SMALL = 1077,
    PIC_ENGINE_V8_LARGE = 1078,
    PIC_MANIFOLD_TRI_POWER = 1079,
    PIC_MANIFOLD_TWO_BARREL_V6 = 1080,
    PIC_MANIFOLD_TWO_BARREL_V8 = 1081,
    PIC_MANIFOLD_FOUR_BARREL = 1082,
    PIC_MANIFOLD_DUAL_QUAD = 1083,
    PIC_CARB_TWO_BARREL = 1084,
    PIC_CARB_FOUR_BARREL = 1085,
    PIC_CARB_RACING = 1086,
    PIC_BOLT_TURN_1 = 1087,
    PIC_BOLT_TURN_2 = 1088,
    PIC_BOLT_TURN_3 = 1089,
    PIC_GAS_STATION = 1090,
    PIC_CORVETTE_DISTANT_REAR = 1091,
    PIC_SEDAN_DISTANT_REAR = 1092,
    PIC_BOBS_DRIVE_IN = 1093,
    PIC_IGNITION_WIRES_DISCONNECTED = 1094,
    PIC_IGNITION_WIRES_CONNECTED = 1095,
    PIC_GAS_STATION_MASK = 1096,
    PIC_PUMP_WITHOUT_NOZZLE = 1097,
    PIC_PUMP_MASK_WITHOUT_NOZZLE = 1098,
    PIC_TRANS_AUTOMATIC = 1099,
    PIC_TRANS_THREE_SPEED = 1100,
    PIC_TRANS_FOUR_SPEED = 1101,
    PIC_TRANS_RACING = 1102,
    PIC_TRANSMISSION_BOLT_TURNS = 1103,
    PIC_MANIFOLD_RACING = 1104,
    PIC_TRANSMISSION_LINKAGE = 1105,
    PIC_TIRE_SLICK_1 = 1106,
    PIC_TIRE_SLICK_2 = 1107,
    PIC_TIRE_SLICK_3 = 1108,
    PIC_TIRE_SLICK_4 = 1109,
    PIC_SPEEDING_TICKET = 1110,
    PIC_DODGE_62_POLARA_ROOF = 1111,
    PIC_DODGE_62_POLARA_BODY = 1112,
    PIC_PLYMOUTH_62_SAVOY_BODY = 1113,
    PIC_PLYMOUTH_62_SAVOY_REAR = 1114,
    PIC_PLYMOUTH_62_SAVOY_FRONT = 1115,
    PIC_PLYMOUTH_62_SAVOY_ROOF = 1116,
    PIC_FORD_32_SEDAN_ROOF = 1117,
    PIC_FORD_32_SEDAN_BODY = 1118,
    PIC_FORD_40_DELUXE_BODY = 1119,
    PIC_FORD_40_DELUXE_REAR_BUMPER = 1120,
    PIC_FORD_40_DELUXE_FRONT_BUMPER = 1121,
    PIC_FORD_40_DELUXE_ROOF = 1122,
    PIC_FORD_55_FAIRLANE_REAR = 1123,
    PIC_FORD_55_FAIRLANE_FRONT = 1124,
    PIC_FORD_55_FAIRLANE_ROOF = 1125,
    PIC_FORD_55_FAIRLANE_BODY = 1126,
    PIC_FORD_57_FAIRLANE_BODY = 1127,
    PIC_FORD_57_FAIRLANE_REAR = 1128,
    PIC_FORD_57_FAIRLANE_FRONT = 1129,
    PIC_FORD_57_FAIRLANE_ROOF = 1130,
    PIC_MERCURY_54_MONTEREY_BODY = 1131,
    PIC_MERCURY_54_MONTEREY_REAR = 1132,
    PIC_MERCURY_54_MONTEREY_FRONT = 1133,
    PIC_MERCURY_54_MONTEREY_ROOF = 1134,
    PIC_MERCURY_56_CUSTOM_BODY = 1135,
    PIC_MERCURY_56_CUSTOM_REAR = 1136,
    PIC_FUEL_NOZZLE = 1137,
    PIC_FUEL_CAP = 1138,
    PIC_FUEL_GAUGE = 1139,
    PIC_CARHOP_SILHOUETTES = 1140,
    PIC_CARHOP_WALK_LEFT = 1141,
    PIC_CARHOP_WALK_RIGHT = 1142,
    PIC_MERCURY_56_CUSTOM_FRONT = 1143,
    PIC_MERCURY_56_CUSTOM_ROOF = 1144,
    PIC_CHEVY_49_STYLELINE_BODY = 1145,
    PIC_CHEVY_49_STYLELINE_REAR = 1146,
    PIC_CHEVY_49_STYLELINE_FRONT = 1147,
    PIC_CHEVY_49_STYLELINE_ROOF = 1148,
    PIC_CHEVY_58_IMPALA_BODY = 1149,
    PIC_CHEVY_58_IMPALA_REAR = 1150,
    PIC_CHEVY_58_IMPALA_FRONT_BUMPER = 1151,
    PIC_CHEVY_58_IMPALA_ROOF = 1152,
    PIC_OLDS_49_88_REAR_BUMPER = 1153,
    PIC_OLDS_49_88_FRONT_BUMPER = 1154,
    PIC_OLDS_49_88_ROOF = 1155,
    PIC_OLDS_49_88_BODY = 1156,
    PIC_OLDS_52_88_BODY = 1157,
    PIC_OLDS_52_88_REAR_BUMPER = 1158,
    PIC_OLDS_52_88_FRONT_BUMPER = 1159,
    PIC_OLDS_52_88_ROOF = 1160,
    PIC_PONTIAC_50_STREAK_REAR = 1161,
    PIC_PONTIAC_50_STREAK_FRONT = 1162,
    PIC_PONTIAC_50_STREAK_ROOF = 1163,
    PIC_PONTIAC_50_STREAK_BODY = 1164,
    PIC_TBIRD_57_BODY = 1165,
    PIC_TBIRD_57_FRONT_BUMPER = 1166,
    PIC_TBIRD_57_REAR_BUMPER = 1167,
    PIC_VALIANT_61_V200_REAR = 1168,
    PIC_VALIANT_61_V200_FRONT_BUMPER = 1169,
    PIC_VALIANT_61_V200_ROOF = 1170,
    PIC_VALIANT_61_V200_BODY = 1171,
    PIC_VALIANT_63_V100_BODY = 1172,
    PIC_VALIANT_63_V100_REAR = 1173,
    PIC_VALIANT_63_V100_FRONT_BUMPER = 1174,
    PIC_VALIANT_63_V100_ROOF = 1175,
    PIC_CHEVY_55_BEL_AIR_REAR = 1176,
    PIC_CHEVY_55_BEL_AIR_FRONT = 1177,
    PIC_CHEVY_55_BEL_AIR_ROOF = 1178,
    PIC_CHEVY_55_BEL_AIR_BODY = 1179,
    PIC_DODGE_55_LANCER_FRONT_BUMPER = 1180,
    PIC_CAR_SHADOW = 1181,
    PIC_ENGINE_OVERLAY_CHEVY_38 = 1182,
    PIC_ENGINE_OVERLAY_CORVETTE_56 = 1183,
    PIC_ENGINE_OVERLAY_CHEVY_40 = 1184,
    PIC_ENGINE_TOP_FORD_32_DEUCE = 1185,
    PIC_STICKER_ROAD_RUNNER = 1186,
    PIC_STICKER_TWO_NOT_ONE = 1187,
    PIC_STICKER_LOVE_MACHINE = 1188,
    PIC_STICKER_DEAD_END_KIDS = 1189,
    PIC_STICKER_HOT_ROD = 1190,
    PIC_STICKER_LOST_MARBLES = 1191,
    PIC_STICKER_RINGER = 1192,
    PIC_STICKER_HOT_LIPS = 1193,
    PIC_PORTRAIT_RALPH = 1194,
    PIC_PORTRAIT_EDDIE = 1195,
    PIC_PORTRAIT_MIKE = 1196,
    PIC_PORTRAIT_BIFF = 1197,
    PIC_PORTRAIT_BUTCH = 1198,
    PIC_PORTRAIT_CHARLIE = 1199,
    PIC_STICKER_MENU = 1200,
    PIC_KINGS_CORVETTE_63_BODY = 1201,
    PIC_PORTRAIT_JIM = 1202,
    PIC_PORTRAIT_STEVE = 1203,
    PIC_PORTRAIT_ERICA = 1204,
    PIC_PORTRAIT_GEORGE = 1205,
    PIC_PORTRAIT_TOM = 1206,
    PIC_PORTRAIT_CLYDE = 1207,
    PIC_PORTRAIT_ERNIE = 1208,
    PIC_PORTRAIT_KIRK = 1209,
    PIC_PORTRAIT_WALLY = 1210,
    PIC_PORTRAIT_DICK = 1211,
    PIC_PORTRAIT_SPIKE = 1212,
    PIC_PORTRAIT_CHIP = 1213,
    PIC_PORTRAIT_BILL = 1214,
    PIC_PORTRAIT_VINNIE = 1215,
    PIC_PORTRAIT_JOHN = 1216,
    PIC_PORTRAIT_KINGS_GIRLFRIEND = 1217,
    PIC_STICKER_WIPE = 1218,
    PIC_CALENDAR_JUNE_JULY = 1219,
    PIC_CALENDAR_DAY_OUTLINE = 1220,
    PIC_CALENDAR_AUGUST_SEPTEMBER = 1221,
    PIC_TUNING_BUTTONS = 1222,
    PIC_ROLLING_TIRE = 1223,
    PIC_PUMP_WITH_NOZZLE = 1224,
    PIC_CARHOP_IDLE = 1225,
    PIC_PUMP_MASK_WITH_NOZZLE = 1226,
    PIC_SHIFTER_THREE_SPEED_GATE = 1227,
    PIC_SHIFTER_FOUR_SPEED_GATE = 1228,
    PIC_SHIFTER_AUTOMATIC_GATE = 1229,
    PIC_SHIFTER_THREE_SPEED_KNOB = 1230,
    PIC_SHIFTER_FOUR_SPEED_KNOB = 1231,
    PIC_SHIFTER_AUTOMATIC_KNOB = 1232,
    PIC_GAS_ATTENDANT = 1233,
    PIC_TIRE_SHADOW = 1234,
    PIC_TBIRD_57_ROOF = 1235,
    PIC_SIGN_BOBS = 1236,
    PIC_SIGN_GARAGE = 1237,
    PIC_SIGN_GAS_STATION = 1238,
    PIC_SEDAN_SMALL_REAR = 1239,
    PIC_AUTOMATIC_DASH_GAUGE = 1240,
    PIC_POLICE_REAR_ANGLED = 1241,
    PIC_POLICE_REAR = 1242,
    PIC_MIRROR_LANDSCAPE = 1243,
    PIC_PORTRAIT_THE_KING = 1244,
    PIC_POLICE_PASSING = 1245,
    PIC_POLICE_DISTANT_REAR = 1246,
    PIC_COUPE_REAR_ANGLED = 1247,
    PIC_COUPE_REAR = 1248,
    PIC_HOT_ROD_REAR_ANGLED = 1249,
    PIC_HOT_ROD_REAR = 1250,
    PIC_HOT_ROD_DISTANT_REAR = 1251,
    PIC_SPORTS_CAR_REAR = 1252,
    PIC_SPORTS_CAR_REAR_ANGLED = 1253,
    PIC_SPORTS_CAR_DISTANT_REAR = 1254,
    PIC_JAIL_WALL = 1255,
    PIC_JAIL_CAR_WINDOW = 1256,
    PIC_JAIL_BAR = 1257,
    PIC_SIGN_COUNTY_LINE = 1258,
    PIC_CRASHED_WINDSHIELD = 1259,
    PIC_CRASH_WINDOW_CORNER = 1260,
    PIC_SIGN_VEGAS_GAMBLER = 1261,
    PIC_SIGN_BLOCK_OUT = 1262,
    PIC_TUNING_HAND = 1263,
    PIC_CRASH_BLOOD_LEFT = 1264,
    PIC_CRASH_BLOOD_RIGHT = 1265,
    PIC_CRASH_BLOOD_CENTER = 1266,
    PIC_COUNTRY_SKYLINE = 1267
};

/** Combined entry count required when validating both picture libraries. */
#define PICTURE_COUNT 277
/**
 * @name Second picture library numbering
 * @brief Subtract the bias to convert IDs to shared slot indices.
 * @{
 */
#define PICTURE_SECOND_BASE 1000
#define PICTURE_SECOND_INDEX_BIAS 991
/** @} */

struct Bitmap;

/** @brief Manufacturer compatibility bits for cars and catalog parts. */
enum PartFit {
    PART_FIT_GM = 1,       /**< General Motors. */
    PART_FIT_FORD = 2,     /**< Ford and Mercury. */
    PART_FIT_CHRYSLER = 4, /**< Chrysler, Dodge, Plymouth and Valiant. */
    PART_FIT_ALL = 7       /**< Fits all three manufacturers. */
};

/** Manufacturer compatibility bits in the stock equipment word. */
#define SPEC_FIT_MASK 0x000f
/**
 * @name Tire stock variant
 * @brief Right-shift the stock equipment word, then mask to obtain the variant.
 * @{
 */
#define SPEC_TIRES_SHIFT 4
#define SPEC_TIRES_MASK 3
/** @} */
/**
 * @name Transmission stock variant
 * @brief Right-shift the stock equipment word, then mask to obtain the variant.
 * @{
 */
#define SPEC_TRANS_SHIFT 6
#define SPEC_TRANS_MASK 3
/** @} */
/**
 * @name Engine stock variant
 * @brief Right-shift the stock equipment word, then mask to obtain the variant.
 * @{
 */
#define SPEC_ENGINE_SHIFT 8
#define SPEC_ENGINE_MASK 7
/** @} */
/**
 * @name Manifold stock variant
 * @brief Right-shift the stock equipment word, then mask to obtain the variant.
 * @{
 */
#define SPEC_MANIFOLD_SHIFT 11
#define SPEC_MANIFOLD_MASK 7
/** @} */
/**
 * @name Carburetor stock variant
 * @brief Right-shift the stock equipment word, then mask to obtain the variant.
 * @{
 */
#define SPEC_CARB_SHIFT 14
#define SPEC_CARB_MASK 3
/** @} */

/**
 * @brief A car model's price, performance rank and stock equipment.
 *
 * Ten-byte records loaded from the game data file; the last is the King's car.
 */
struct CarSpec {
    int basePrice;      /**< Purchase price and won-car value, in dollars. */
    unsigned char rank; /**< Model performance rank, 0..8; higher is better. */
    signed char nextAd; /**< Next model index in the classifieds; negative ends. */
    /** Packed stock variants and @ref PartFit bits; see @ref SPEC_FIT_MASK. */
    unsigned stockParts;
    int nameOffset;         /**< Model-name byte offset in description text. */
    enum PictureId bodyPic; /**< Base car-body picture. */
};

/** @brief Four-view picture-table rows selected by the car model. */
enum RaceSpriteSet {
    RACE_SPRITES_KING = 0,   /**< Unique Corvette passing and rear views. */
    RACE_SPRITES_SEDAN,      /**< Sedan passing and rear views. */
    RACE_SPRITES_COUPE,      /**< Uses sedan passing and distant views. */
    RACE_SPRITES_HOT_ROD,    /**< Uses the sedan passing view. */
    RACE_SPRITES_SPORTS_CAR, /**< Uses the sedan passing view. */
    RACE_SPRITE_SET_COUNT    /**< Row count in the opponent picture table. */
};

/**
 * @brief A car model's race sprites and side-view modification pictures.
 *
 * Ten-byte records loaded from the game data file, indexed by car
 * specification. Roof and bumper pictures also determine which body changes
 * are available and contribute to performance.
 */
struct Model {
    signed char raceSpriteSet; /**< On-road sprites; @ref RaceSpriteSet. */
    char unused;               /**< Unused byte from the game data file. */
    /** Chopped-roof patch; zero if unavailable. */
    enum PictureId choppedRoofPic;
    /** Large-engine overlay; zero for the King. */
    enum PictureId engineOverlayPic;
    /** Rear-bumper removal patch; zero if stripping is unavailable. */
    enum PictureId rearBumperRemovalPic;
    /** Front-bumper removal patch; zero if stripping is unavailable. */
    enum PictureId frontBumperRemovalPic;
};

/** @brief Car-image rectangle used for model-specific hit targets. */
struct CarHotRect {
    unsigned char x; /**< Left offset in car-image pixels. */
    unsigned char y; /**< Top offset in car-image rows, positive downwards. */
    unsigned char w; /**< Horizontal extent in pixels. */
    unsigned char h; /**< Vertical extent in rows. */
};

/** @brief Model-specific car interaction regions, in data-file order. */
enum CarRegion {
    CAR_REGION_REAR_BUMPER,  /**< Rear bumper. */
    CAR_REGION_ROOF,         /**< Roof. */
    CAR_REGION_TRANSMISSION, /**< Transmission. */
    CAR_REGION_HOOD,         /**< Hood. */
    CAR_REGION_FRONT_BUMPER, /**< Front bumper. */
    CAR_REGION_COUNT         /**< Number of regions per model. */
};

/**
 * @brief Interactive rectangles on one model's side-view car image.
 *
 * Loaded from the game data file and translated to the displayed car origin.
 */
struct CarHotspots {
    /** Indexed by @ref CarRegion. */
    struct CarHotRect regions[CAR_REGION_COUNT];
};

/**
 * @brief Pixel anchors for assembling one model's side-view car image.
 *
 * Coordinates are relative to the car image, with Y positive downwards.
 */
struct CarPicLayout {
    unsigned char wheelX1;   /**< First tire image's left X. */
    unsigned char wheelX2;   /**< Second tire image's left X. */
    unsigned char portraitX; /**< Portrait image's left X. */
    /** Inclusive portrait bottom row before the chopped-roof correction. */
    unsigned char portraitBottom;
    unsigned char roofX;        /**< Roof patch X; top row is zero. */
    unsigned char engineX;      /**< Large-engine overlay's left X. */
    unsigned char engineY;      /**< Large-engine overlay's top row. */
    unsigned char rearBumperY;  /**< Rear-removal patch's top row; X is zero. */
    unsigned char frontBumperX; /**< Front-removal patch's left X. */
    unsigned char frontBumperY; /**< Front-removal patch's top row. */
    unsigned char stickerX;     /**< Sticker-background origin X. */
    unsigned char stickerY;     /**< Sticker-background origin row. */
};

/** @brief Gas-station service anchors in car-image pixels, Y downwards. */
struct ServicePoints {
    unsigned char fuelCapX; /**< Fuel cap X. */
    unsigned char fuelCapY; /**< Fuel cap row. */
    /** Rear-window X; either zero coordinate hides it. */
    unsigned char rearGlassX;
    unsigned char rearGlassY; /**< Rear-window row. */
    /** Windshield X; zero skips window translation. */
    unsigned char frontGlassX;
    unsigned char frontGlassY; /**< Windshield row. */
};

/** @brief One sticker picture's placement within the car sticker background. */
struct StickerPos {
    enum PictureId pictureId; /**< Sticker picture. */
    int dx;                   /**< Pixel offset rightwards within background. */
    int dy;                   /**< Row offset downwards within background. */
};
#include "MAIN.H"

/** @brief Borrowed path and state pointers for the selected picture library. */
struct PicLibSelection {
    int *entryCount;  /**< Selected library's directory-entry count. */
    int *handle;      /**< Selected DOS handle; zero means closed. */
    char *path;       /**< Selected drive-prefixed library path. */
    int *otherHandle; /**< Other handle, closed when opening this file. */
};

extern unsigned char g_rleCounts[16];
extern unsigned char g_rleValues[16];
extern int g_picNewHeader;
extern struct CarHotspots g_bodyRects[];
extern struct CarPicLayout g_layoutAutoSprArray[];
extern struct StickerPos g_imageDecalLayout[];
extern int g_usedCarAdFirst;
extern unsigned g_carRatingsTable[][3];
extern struct CarSpec g_carData[];
extern struct ServicePoints g_pointsModelService[];
extern unsigned g_rivalStakeToleranceTable[][3];
extern int g_lib1Count;
extern int g_countLibBmB;
extern char *g_library1Path;
extern char *g_picLibBPath;
extern int g_acceptProbability[][3];
extern struct Model g_models[];
int di_seek(int handle, long pos);
int di_read_index(int n);
void di_free_pic(enum PictureId n);
struct Bitmap far *di_unpack_pic(enum PictureId id, struct Bitmap far *bm);
void di_pic_size(enum PictureId id, int *w, int *h, int *x);
struct Bitmap far *di_bm(enum PictureId id, int mode);
int di_cache_list(int *list);
int di_load_list(int *list, int n);
void di_free_list(int *list);
void di_open_libs(int a, enum VideoChoice mode);
void di_load_data(void);
int di_data_drive(void);
void di_load_hall(void);
void di_save_hall(void);

#endif
