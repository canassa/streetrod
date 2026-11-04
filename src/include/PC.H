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

#define PPI_B 0x61
#define PIT_CH0 0x40
#define PIT_CH2 0x42
#define PIT_CMD 0x43
#define PIC_CMD 0x20
#define GAME_PORT 0x201
#define GC_INDEX 0x3ce
#define GC_DATA 0x3cf
#define SEQ_INDEX 0x3c4
#define SEQ_DATA 0x3c5
#define CRTC_INDEX 0x3d4
#define CRTC_DATA 0x3d5
#define INPUT_STATUS 0x3da
#define MONO_STATUS 0x3ba
#define MONO_MODE 0x3b8
#define CGA_MODE 0x3d8
#define TANDY_PAGE 0x3df

#define GC_COLOR_COMPARE 2
#define GC_READ_MAP 4
#define GC_MODE 5
#define GC_BIT_MASK 8
#define GC_WRITE_MODE0 0
#define GC_WRITE_MODE1 1
#define GC_WRITE_MODE2 2
#define GC_READ_MODE1 0x08
#define GC_MASK_ALL 0xff

#define SEQ_MAP_MASK 2
#define SEQ_PLANE0 0x01
#define SEQ_PLANE1 0x02
#define SEQ_PLANE2 0x04
#define SEQ_PLANE3 0x08
#define SEQ_ALL_PLANES 0x0f

#define CRTC_START_HI 0x0c
#define CRTC_START_LO 0x0d
#define CRTC_OFFSET 0x13
#define STATUS_VRETRACE 0x08
#define HERC_STATUS_VSYNC 0x80
#define MODE_CTRL_GRAPHICS 0x02
#define MODE_CTRL_VIDEO 0x08
#define MODE_CTRL_BLINK 0x20

#define PPI_SPEAKER_GATE 0x01
#define PPI_SPEAKER_DATA 0x02
#define PPI_SPEAKER_MASK (PPI_SPEAKER_GATE | PPI_SPEAKER_DATA)
#define PIT_CH0_LOHI_MODE3 0x36
#define PIT_CH2_LOHI_MODE3 0xb6
#define PIC_EOI 0x20
#define GAME_BUTTON_MASK 0xf0
#define GAME_JOY1_AXES 0x03

#endif
