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

#define BIOS_VIDEO 0x10
#define DOS_INT 0x21
#define MOUSE_INT 0x33
#define TIMER_INT 8

#define VIDEO_SET_MODE 0x00
#define VIDEO_SET_PAGE 0x05
#define VIDEO_SET_PALETTE 0x0b
#define VIDEO_PALETTE_REGS 0x10
#define VIDEO_GET_DCC 0x1a
#define DCC_READ 0x00
#define DCC_VGA_COLOR 8

#define MODE_TEXT80 0x03
#define MODE_CGA_320_200_4 0x04
#define MODE_TANDY_320_200_16 0x09
#define MODE_EGA_320_200_16 0x0d

#define CGA_SELECT_PALETTE 1
#define CGA_PALETTE_GREEN_RED_BROWN 0
#define CGA_PALETTE_CYAN_MAGENTA_WHITE 1
#define PALETTE_SET_ALL 0x02
#define EGA_PALETTE_LAST 0x0f

#define DOS_WRITE_STRING 0x09
#define DOS_SEEK 0x42
#define DOS_SEEK_START 0x00

#define MOUSE_RESET 0x0000
#define MOUSE_GET_POSITION 0x0003
#define MOUSE_READ_MOTION 0x000b
#define MOUSE_LEFT_BUTTON 0x01
#define MOUSE_RIGHT_BUTTON 0x02

#endif
