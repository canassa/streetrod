/* F_057DC @ 0000:57DC (linear 0x057DC, 146 bytes) */

#include <dos.h>
#include "GLOBALS.H"
#include "PROTOS.H"

static struct Node * near F_03C1B(void);

static int near F_057DC(int handle, struct Node **head)
{
    struct Node *p;
    struct Node *prev;
    struct Node buf;
    unsigned n;

    prev = 0;
    if (*head == 0)
        return 0;
    *head = 0;
    for (;;) {
        if (_dos_read(handle, &buf, 8, &n) != 0 || n != 8)
            return 1;
        p = F_03C1B();
        *p = buf;
        if (prev) {
            prev->next = p;
            prev = p;
        } else
            *head = prev = p;
        if (prev->next == 0)
            break;
        prev->next = 0;
    }
    return 0;
}
