#include "STRUCTS.H"
#include "SCENERY2.H"
#include "SPANS2.H"

struct List g_sprPole = {7, g_sprPoleLod};   /* DS:60CC */
struct Run g_sprPoleLod[7] = {{5, g_spans2}, {21, g_spans2 + 5}, {34, g_spans2 + 26}, {48, g_spans2 + 60}, {64, g_spans2 + 108}, {75, g_spans2 + 172}, {95, g_spans2 + 247}};   /* DS:60A2 */
struct List g_sprTree2 = {7, g_sprTree2Lod};   /* DS:60FA */
struct Run g_sprTree2Lod[7] = {{21, g_spans2 + 342}, {35, g_spans2 + 363}, {58, g_spans2 + 398}, {87, g_spans2 + 456}, {132, g_spans2 + 543}, {181, g_spans2 + 675}, {199, g_spans2 + 856}};   /* DS:60D0 */
struct List g_sprTree3 = {7, g_sprTree3Lod};   /* DS:6128 */
struct Run g_sprTree3Lod[7] = {{18, g_spans2 + 1055}, {33, g_spans2 + 1073}, {71, g_spans2 + 1106}, {121, g_spans2 + 1177}, {167, g_spans2 + 1298}, {229, g_spans2 + 1465}, {306, g_spans2 + 1694}};   /* DS:60FE */
struct List g_sprArrowSignL = {7, g_sprArrowSignLLod};   /* DS:6156 */
struct Run g_sprArrowSignLLod[7] = {{7, g_spans2 + 2000}, {17, g_spans2 + 2007}, {35, g_spans2 + 2024}, {53, g_spans2 + 2059}, {73, g_spans2 + 2112}, {105, g_spans2 + 2185}, {127, g_spans2 + 2290}};   /* DS:612C */
struct List g_sprCityLimits = {7, g_sprCityLimitsLod};   /* DS:6184 */
struct Run g_sprCityLimitsLod[7] = {{12, g_spans2 + 2417}, {38, g_spans2 + 2429}, {77, g_spans2 + 2467}, {103, g_spans2 + 2544}, {129, g_spans2 + 2647}, {161, g_spans2 + 2776}, {195, g_spans2 + 2937}};   /* DS:615A */
struct List g_sprArrowSignUp = {7, g_sprArrowSignUpLod};   /* DS:61B2 */
struct Run g_sprArrowSignUpLod[7] = {{7, g_spans2 + 3132}, {19, g_spans2 + 3139}, {35, g_spans2 + 3158}, {51, g_spans2 + 3193}, {71, g_spans2 + 3244}, {103, g_spans2 + 3315}, {127, g_spans2 + 3418}};   /* DS:6188 */
struct List g_sprNarrowSign = {7, g_sprNarrowSignLod};   /* DS:61E0 */
struct Run g_sprNarrowSignLod[7] = {{21, g_spans2 + 3545}, {42, g_spans2 + 3566}, {50, g_spans2 + 3608}, {74, g_spans2 + 3658}, {109, g_spans2 + 3732}, {145, g_spans2 + 3841}, {157, g_spans2 + 3986}};   /* DS:61B6 */
struct List g_sprChevronL = {7, g_sprChevronLLod};   /* DS:620E */
struct Run g_sprChevronLLod[7] = {{8, g_spans2 + 4143}, {17, g_spans2 + 4151}, {27, g_spans2 + 4168}, {41, g_spans2 + 4195}, {60, g_spans2 + 4236}, {71, g_spans2 + 4296}, {84, g_spans2 + 4367}};   /* DS:61E4 */
struct List g_sprChevronR = {7, g_sprChevronRLod};   /* DS:623C */
struct Run g_sprChevronRLod[7] = {{8, g_spans2 + 4451}, {17, g_spans2 + 4459}, {27, g_spans2 + 4476}, {41, g_spans2 + 4503}, {60, g_spans2 + 4544}, {71, g_spans2 + 4604}, {83, g_spans2 + 4675}};   /* DS:6212 */
struct List g_sprSCurveSign = {7, g_sprSCurveSignLod};   /* DS:626A */
struct Run g_sprSCurveSignLod[7] = {{10, g_spans2 + 4758}, {25, g_spans2 + 4768}, {37, g_spans2 + 4793}, {48, g_spans2 + 4830}, {65, g_spans2 + 4878}, {86, g_spans2 + 4943}, {101, g_spans2 + 5029}};   /* DS:6240 */
struct List g_sprSCurveSign2 = {7, g_sprSCurveSign2Lod};   /* DS:6298 */
struct Run g_sprSCurveSign2Lod[7] = {{10, g_spans2 + 5130}, {25, g_spans2 + 5140}, {37, g_spans2 + 5165}, {48, g_spans2 + 5202}, {65, g_spans2 + 5250}, {86, g_spans2 + 5315}, {101, g_spans2 + 5401}};   /* DS:626E */
