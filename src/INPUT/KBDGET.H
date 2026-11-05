/**
 * @file KBDGET.H
 * @brief Keyboard event retrieval and interrupt installation.
 */

#ifndef KBDGET_H
#define KBDGET_H

/** No pending keyboard event; also clears the event latch. */
#define KEY_NONE 0

/** Text-entry terminator and race-start confirmation. */
#define ASCII_ENTER 0x0d

/** Cancel text entry or request a driving exit. */
#define ASCII_ESCAPE 0x1b

/** Immediate quit request outside raw text capture. */
#define ASCII_DELETE 0x7f

/** Primary action button event, also accepted to leave driving. */
#define KEY_FIRE 0x1f

char kg_key(void);
void kg_hook(void);

#endif
