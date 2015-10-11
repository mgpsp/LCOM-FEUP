
#include <minix/drivers.h>
#include <machine/int86.h>
#include <minix/syslib.h>

#include "test5.h"
#include "vbe.h"
#include "video_gr.h"
#include "keyboard.h"
#include "timer.h"
#include "lmlib.h"

////////////////

#define VIDEO_CARD				0x10
#define GET_VBE_INFO			0x4F00
#define InfoBlock_SIZE		sizeof(vbe_info_t)
#define PB2BASE(x)				(((x) >> 4) & 0x0F000)
#define PB2OFF(x) 				((x) & 0x0FFFF)

////////////////

void *test_init(unsigned short mode, unsigned short delay)
{
	char *video_mem;
	video_mem = vg_init(mode);
	vbe_mode_info_t vmi_p;

	if (vbe_get_mode_info(mode, &vmi_p) != 0)
	{
		printf("\tvg_init(): vbe_get_mode_info() failed \n");
		return NULL;
	}

	timer_test_int(delay);

	if (vg_exit() != 0)
	{
		printf("\ttest_init(): vg_exit() failed\n");
		return NULL;
	}
	
	printf("\nPhysical address of the Video RAM (VRAM): 0x%x\n", vmi_p.PhysBasePtr);
	return video_mem;
}

int test_square(unsigned short x, unsigned short y, unsigned short size, unsigned long color)
{
	if (vg_square(x, y, size, color) != 0)
	{
		printf("\ttest_square(): vg_square() failed\n");
		return 1;
	}
	return 0;
}

int test_line(unsigned short xi, unsigned short yi, 
		           unsigned short xf, unsigned short yf, unsigned long color)
{
	if (vg_line(xi, yi, xf, yf, color) != 0)
	{
		printf("\ttest_line(): vg_line() failed\n");
		return 1;
	}
	return 0;
}

int test_xpm(unsigned short xi, unsigned short yi, char *xpm[])
{
	if (vg_xpm(xi, yi, xpm) != 0)
	{
		printf("\ttest_xpm(): vg_xpm() failed\n");
		return 1;
	}
	return 0;
}	

int test_move(unsigned short xi, unsigned short yi, char *xpm[], 
				unsigned short hor, short delta, unsigned short time)
{
	if (vg_move(xi, yi, xpm, hor, delta, time) != 0)
	{
		printf("\ttest_move(): vg_move() failed\n");
		return 1;
	}
	return 0;
}					

int test_controller()
{
	mmap_t map;
	struct reg86u r;
	vbe_info_t vi;
	//char sig[4] = "VBE2";
	//vi->VbeSignature = sig;
	int i = 0;
	char *lPtr;

	if (lm_init() == NULL)
	{
		printf("\tvtest_controller():lm_init() failed \n");
		return 1;
	}

	lm_alloc(InfoBlock_SIZE, &map);

	r.u.w.ax = GET_VBE_INFO;
	r.u.w.es = PB2BASE(map.phys);
	r.u.w.di = PB2OFF(map.phys);
	r.u.b.intno = VIDEO_CARD;

	if (sys_int86(&r) != OK)
	{
		printf("\ttest_controller(): sys_int86() failed \n");
		return 1;
	}

	vi = *(vbe_info_t*) map.virtual;

	lm_free(&map);

	return 0;
}

