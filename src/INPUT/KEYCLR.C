#include "STRUCTS.H"
#include "KEYCLR.H"
#include "M_02B9A.H"

/* kc_flush @ 0000:2DB0 (linear 0x02DB0, 6 bytes) */

void kc_flush(void)
{
    g_lastKey = 0;
}
