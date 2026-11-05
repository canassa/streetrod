/* Far working storage: 277 PictureSlot records and five 1120-word terrain arrays. */
#include "SLOTS.H"

struct PictureSlot far g_slot[277];
unsigned far g_roadsideLeftBits[1120];
unsigned far g_roadsideRightBits[1120];
int far g_roadCurve[1120];
unsigned far g_roadBitsArray[1120];
int far g_roadHeight[1120];
