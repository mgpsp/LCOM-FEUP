#include <minix/syslib.h>
#include <minix/drivers.h>
#include <machine/int86.h>

#include "vbe.h"
#include "lmlib.h"

////////////////

#define PB2BASE(x)				(((x) >> 4) & 0x0F000)
#define PB2OFF(x) 				((x) & 0x0FFFF)

#define ModeInfoBlock_SIZE		sizeof(vbe_mode_info_t)
#define GET_VBE_MODE_INFO		0x4F01
#define VIDEO_CARD				0x10

#define BIT(n) 					(0x01<<(n))
#define LINEAR_MODEL_BIT 		14

////////////////

int vbe_get_mode_info(unsigned short mode, vbe_mode_info_t *vmi_p)
{
	mmap_t map;
	struct reg86u r;

	if (lm_init() == NULL)
	{
		printf("\tvbe_get_mode_info():lm_init() failed \n");
		return 1;
	}

	lm_alloc(ModeInfoBlock_SIZE, &map);

	r.u.w.ax = GET_VBE_MODE_INFO; /* VBE get mode info */
	/* translate the buffer linear address to a far pointer */
	r.u.w.es = PB2BASE(map.phys); /* set a segment base */
	r.u.w.di = PB2OFF(map.phys); /* set the offset accordingly */
	r.u.w.cx = mode;
	r.u.b.intno = VIDEO_CARD;

	if (sys_int86(&r) != OK)
	{
		printf("\tvbe_get_mode_info(): sys_int86() failed \n");
		return 1;
	}

	*vmi_p = *(vbe_mode_info_t*) map.virtual;

	lm_free(&map);

	return 0;
}


