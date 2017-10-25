#include "display.h"
#include "print.h"

typedef unsigned int   u32;
typedef unsigned short u16;
typedef unsigned char  u8;

struct VBEInfo 
{
	char signature[4];	// must be "VESA" to indicate valid VBE support
	u16 version;			// VBE version; high byte is major version, low byte is minor version
	u32 oem;			// segment:offset pointer to OEM
	u32 capabilities;		// bitfield that describes card capabilities
	u32 video_modes;		// segment:offset pointer to list of supported video modes
	u16 video_memory;		// amount of video memory in 64KB blocks
	u16 software_rev;		// software revision
	u32 vendor;			// segment:offset to card vendor string
	u32 product_name;		// segment:offset to card model name
	u32 product_rev;		// segment:offset pointer to product revision
	char reserved[222];		// reserved for future expansion
	char oem_data[256];		// OEM BIOSes store their strings in this area
} __attribute__((packed));
static_assert(sizeof(VBEInfo) == 512, "size");

struct VBEModeInfo 
{
   u16 attributes;      // deprecated, only bit 7 should be of interest to you, and it indicates the mode supports a linear frame buffer.
   u8  window_a;         // deprecated
   u8  window_b;         // deprecated
   u16 granularity;      // deprecated; used while calculating bank numbers
   u16 window_size;
   u16 segment_a;
   u16 segment_b;
   u32 win_func_ptr;      // deprecated; used to switch banks from protected mode without returning to real mode
   u16 pitch;         // number of bytes per horizontal line
   u16 width;         // width in pixels
   u16 height;         // height in pixels
   u8  w_char;         // unused...
   u8  y_char;         // ...
   u8  planes;
   u8  bpp;         // bits per pixel in this mode
   u8  banks;         // deprecated; total number of banks in this mode
   u8  memory_model;
   u8  bank_size;      // deprecated; size of a bank, almost always 64 KB but may be 16 KB...
   u8  image_pages;
   u8  reserved0;
   u8  red_mask;
   u8  red_position;
   u8  green_mask;
   u8  green_position;
   u8  blue_mask;
   u8  blue_position;
   u8  reserved_mask;
   u8  reserved_position;
   u8  direct_color_attributes;
   u32 framebuffer;      // physical address of the linear frame buffer; write here to draw to the screen
   u32 off_screen_mem_off;
   u16 off_screen_mem_size;   // size of memory in the framebuffer but not being displayed on the screen
   u8  reserved1[206];
} __attribute__((packed));

struct Regsv86
{
	u16 di, si, bp, sp, bx, dx, cx, ax, es;
} __attribute__((packed));
static_assert(sizeof(Regsv86) == 18, "size");

extern "C" void callv86(int int_no, Regsv86& regs);

/*static void vbe_setvideomode(u16 mode)
{
	Regsv86 regs;
	regs.ax = mode;
	callv86(0x10, regs);
}*/

static void vbe_getinfo(VBEInfo& info)
{
	u32 vbeloc = (u32)&info;
	Regsv86 regs = {};
	regs.ax = 0x4F00;
	regs.di = vbeloc & 0xF;
	regs.es = (vbeloc >> 4) & 0xFFFF;
	callv86(0x10, regs);
}

static void vbe_getmodeinfo(u16 mode, VBEModeInfo& info)
{
	u32 infoLoc = (u32)&info;
	Regsv86 regs;
	regs.ax = 0x4F01;
	regs.cx = mode;
	regs.di = infoLoc & 0xF;
	regs.es = (infoLoc >> 4) & 0xFFFF;
	callv86(0x10, regs);
}

static void vbe_setvideomode(u16 mode)
{
	Regsv86 regs;
	regs.ax = 0x4F02;
	regs.bx = mode | 0x4000;
	callv86(0x10, regs);
}

static u16 vbe_findvideomode(int width, int height, int depth)
{
	VBEInfo vbe_info;
	vbe_getinfo(vbe_info);
	printf("Video Memory %? MB\n", (int)(vbe_info.video_memory * 64)/1024);

	u16* video_modes = (u16*)(((vbe_info.video_modes & 0xFFFF0000) >> 12) + (vbe_info.video_modes & 0xFFFF));
	u16 bestMode = 0x274;//default to 640x480 24bits (https://en.wikipedia.org/wiki/VESA_BIOS_Extensions#Other_commonly_available_graphics_modes)
	for (int i = 0; video_modes[i] != 0xFFFF; ++i)
	{
		VBEModeInfo mode_info;
		vbe_getmodeinfo(video_modes[i], mode_info);

		if ((mode_info.attributes & 0x90) != 0x90)
			continue;
		if (mode_info.memory_model != 4 && mode_info.memory_model != 6)
			continue;

		printf("%?x%?x%? ", (int)mode_info.width, (int)mode_info.height, (int)mode_info.bpp);
		if (width == mode_info.width && height == mode_info.height && depth == mode_info.bpp)
			return video_modes[i];
		if (depth == mode_info.bpp && mode_info.width <= width && mode_info.height <= height)
			bestMode = video_modes[i];
	}

	return bestMode;
}

void display_init()
{
	u16 mode = vbe_findvideomode(800, 600, 32);
	printf("mode: %?\n", mode);
	vbe_setvideomode(mode);
	//vbe_setmodeinfo(mode);
}
