/* PC.H - fixed PC memory addresses. */

#ifndef PC_H
#define PC_H

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 200
#define EGA_PITCH 40

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

#define CRIT_DEVICE_ERROR 0x8000
#define CRIT_RETRY_ALLOWED 0x1000
#define CRIT_WRITE_PROTECT 0
#define CRIT_NOT_READY 2
#define CRIT_PAPER_OUT 9

#define MOUSE_RESET 0x00
#define MOUSE_GET_POSITION 0x03
#define MOUSE_READ_MOTION 0x0b
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

#define SC_ESC          0x01
#define SC_1            0x02
#define SC_2            0x03
#define SC_3            0x04
#define SC_4            0x05
#define SC_5            0x06
#define SC_6            0x07
#define SC_7            0x08
#define SC_8            0x09
#define SC_9            0x0a
#define SC_0            0x0b
#define SC_BACKSPACE    0x0e
#define SC_Q            0x10
#define SC_W            0x11
#define SC_E            0x12
#define SC_R            0x13
#define SC_T            0x14
#define SC_Y            0x15
#define SC_U            0x16
#define SC_I            0x17
#define SC_O            0x18
#define SC_P            0x19
#define SC_ENTER        0x1c
#define SC_CTRL         0x1d
#define SC_A            0x1e
#define SC_S            0x1f
#define SC_D            0x20
#define SC_F            0x21
#define SC_G            0x22
#define SC_H            0x23
#define SC_J            0x24
#define SC_K            0x25
#define SC_L            0x26
#define SC_Z            0x2c
#define SC_X            0x2d
#define SC_C            0x2e
#define SC_V            0x2f
#define SC_B            0x30
#define SC_N            0x31
#define SC_M            0x32
#define SC_COMMA        0x33
#define SC_PERIOD       0x34
#define SC_SPACE        0x39
#define SC_F1           0x3b
#define SC_F2           0x3c
#define SC_F3           0x3d
#define SC_F4           0x3e
#define SC_F5           0x3f
#define SC_F6           0x40
#define SC_F7           0x41
#define SC_F8           0x42
#define SC_F9           0x43
#define SC_F10          0x44
#define SC_SCROLL_LOCK  0x46
#define SC_HOME         0x47
#define SC_UP           0x48
#define SC_PGUP         0x49
#define SC_LEFT         0x4b
#define SC_CENTER       0x4c
#define SC_RIGHT        0x4d
#define SC_END          0x4f
#define SC_DOWN         0x50
#define SC_PGDN         0x51

#endif
