#include <stdint.h>
#include <minix/syslib.h>
#include <minix/drivers.h>
#include <machine/int86.h>

#include "VBE.h"
#include "lmlib.h"

int getModeInfo(unsigned short mode, vbe_mode_info_t *vmi_p)
{
	mmap_t map;
	struct reg86u r;

	if (lm_init() == NULL)
	{
		printf("\tgetModeInfo() in VBE.c:lm_init() failed \n");
		return 1;
	}

	lm_alloc(sizeof(vbe_mode_info_t), &map);

	r.u.w.ax = GET_MODE_INFO; /* VBE get mode info */
	/* translate the buffer linear address to a far pointer */
	r.u.w.es = PB2BASE(map.phys); /* set a segment base */
	r.u.w.di = PB2OFF(map.phys); /* set the offset accordingly */
	r.u.w.cx = mode;
	r.u.b.intno = VIDEO_CARD;

	if (sys_int86(&r) != OK)
	{
		printf("\tgetModeInfo() in VBE.c: sys_int86() failed \n");
		return 1;
	}

	*vmi_p = *(vbe_mode_info_t*) map.virtual;

	lm_free(&map);

	return 0;
}
