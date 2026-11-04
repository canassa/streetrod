/* PC.H - fixed PC memory addresses. */

#ifndef PC_H
#define PC_H

#define BIOS_EQUIP ((unsigned char far *)0x410L)
#define BIOS_TICKS ((long far *)0x46cL)
#define MOUSE_VECTOR ((long far *)0xccL)
#define CGA_VRAM ((char far *)0xb8000000L)
#define MONO_VRAM ((char far *)0xb0000000L)
#define EGA_VRAM ((char far *)0xa0000000L)
#define BIOS_ROM ((char far *)0xf0000000L)

#define EQUIP_VIDEO_MASK     0x30
#define EQUIP_VIDEO_COLOR80  0x20
#define SET_EQUIP_VIDEO(m)   (*BIOS_EQUIP &= ~EQUIP_VIDEO_MASK, *BIOS_EQUIP |= (m))

#endif
