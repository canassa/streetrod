/* entry_beats_score @ 0000:32D0 (linear 0x032D0, 36 bytes) */

#include <stdlib.h>
#include "GLOBALS.H"
#include "PROTOS.H"

static int near entry_beats_score(int i)
{
    return atoi(g_table[i].score) > g_score;
}
