#include <minix/syslib.h>
#include <minix/drivers.h>
#include <machine/int86.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/video.h>
#include <math.h>

#include "VBE.h"
#include "Utilities.h"
#include "Graphics.h"
#include "Bitmap.h"

/* Private global variables */

static char *videoMem;		/* Process address to which VRAM is mapped */
static char *buffer;

static unsigned bitsPerPixel;    /* Number of VRAM bits per pixel */
static unsigned vram_size;

int initializeGraphics(unsigned short mode)
{
	struct reg86u reg86;
	vbe_mode_info_t vmi_p;
	struct mem_range mr;

	reg86.u.b.intno = VIDEO_CARD;
	reg86.u.w.ax = SET_VIDEO_MODE;
	reg86.u.w.bx = BIT(LINEAR_MODEL_BIT) | mode;

	if (sys_int86(&reg86) != OK)
	{
		printf("\tinitializeGraphics() in Graphics.c: sys_int86() failed \n");
		return 1;
	}

	if (getModeInfo(mode, &vmi_p) != 0)
	{
		printf("\tinitializeGraphics() in Graphics.c: getModeInfo() failed \n");
		return 1;
	}

	// Set resolution

	hres = vmi_p.XResolution;
	vres = vmi_p.YResolution;
	bitsPerPixel = vmi_p.BitsPerPixel;

	bytesPerPixel = bitsPerPixel / 8;
	if (bytesPerPixel <= 0)
		bytesPerPixel = 1;

	vram_size = hres * vres * bytesPerPixel;

	// Allow memory mapping

	mr.mr_base = (phys_bytes)(vmi_p.PhysBasePtr);
	mr.mr_limit = mr.mr_base + vram_size;

	if (sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr) != OK)
	{
		printf("\tinitializeGraphics() in Graphics.c: sys_privctl() failed \n");
		return 1;
	}

	// Map memory

	videoMem = vm_map_phys(SELF, (void *)mr.mr_base, vram_size);

	if (videoMem == MAP_FAILED)
	{
		printf("\tinitializeGraphics() in Graphics.c: couldn't map video memory \n");
		return 1;
	}

	buffer = (char*) malloc(vram_size);

	return 0;
}

int exitGraphics()
{
	struct reg86u reg86;

	reg86.u.b.intno = VIDEO_CARD;
	reg86.u.w.ax = SET_TEXT_MODE;

	if (sys_int86(&reg86) != OK)
	{
		printf("\texitGraphics() in Graphics.c: sys_int86() failed \n");
		return 1;
	}
	else
		return 0;
}

int fillDisplay(unsigned long color)
{
	int i, j;
	char *ptr;
	unsigned long tmp;

	ptr = buffer;

	for (i = 0; i < hres * vres; i++)
	{
		tmp = color;
		for (j = 0; j < bytesPerPixel; j++)
		{
			*ptr = (char) tmp;
			ptr++;
			tmp >>= 8;
		}
	}

	return 0;
}

int swapBuffer()
{
	memcpy(videoMem, buffer, vram_size);
	return 0;
}

char* getGraphicsBuffer()
{
	return buffer;
}
