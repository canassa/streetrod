/* Far working storage: 277 Slot records and five 1120-word terrain arrays.
   Uninitialized far definitions are communal variables (COMDEF): LINK allocates them in
   FAR_BSS after every object, in its symbol-hash bucket order. The original order (slots,
   then the five arrays) therefore fixes the order of the names' buckets. The names are
   placeholders chosen to satisfy it (g_slot 64, g_terrain0..4 158..190); the historical
   names are unknown. */
#include "GLOBALS.H"

struct Slot far g_slot[277];
unsigned far g_terrain0[1120];
unsigned far g_terrain1[1120];
int far g_terrain2[1120];
unsigned far g_terrain3[1120];
int far g_terrain4[1120];
