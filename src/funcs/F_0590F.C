/* F_0590F @ 0000:590F (linear 0x0590F, 231 bytes) */

#include <dos.h>
#include "GLOBALS.H"
#include "PROTOS.H"

static int near F_057DC(int handle, struct Node **head);
static struct Node2 * near F_03FB1(void);

static int near F_0590F(int handle, struct Car **head, int flag)
{
    struct Car *car;
    struct Car *prev;
    int m;
    struct Car buf;
    unsigned n;

    prev = 0;
    if (*head == 0)
        return 0;
    *head = 0;
    for (;;) {
        if (_dos_read(handle, &buf, 0x28, &n) != 0 || n != 0x28 || F_057DC(handle, &buf.slot6))
            return 1;
        for (m = 0; m < 5; m++)
            if (F_057DC(handle, &buf.rack.s[m]))
                return 1;
        if (F_057DC(handle, &buf.slot12))
            return 1;
        car = (struct Car *)F_03FB1();
        *car = buf;
        if (prev) {
            prev->f26 = car;
            prev = car;
        } else
            *head = prev = car;
        if (prev->f26 == 0)
            break;
        if (flag)
            break;
        prev->f26 = 0;
    }
    return 0;
}
