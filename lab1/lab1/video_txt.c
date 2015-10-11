#include <minix/drivers.h>
#include <sys/video.h>
#include <sys/mman.h>

#include <assert.h>

#include "vt_info.h"

#include "video_txt.h"

/* Private global variables */

static char *video_mem;		/* Address to which VRAM is mapped */

static unsigned scr_width;	/* Width of screen in columns */
static unsigned scr_lines;	/* Height of screen in lines */

void vt_fill(char ch, char attr)
{
	int i;
	char *ptr;
	ptr = video_mem;
	for(i = 0; i < scr_width * scr_lines * 2; i++, ptr++)
	{
		*ptr = ch;
		ptr++;
		*ptr = attr;
	}
}

void vt_blank()
{
	int i;
	char *ptr;
	ptr = video_mem;
	for(i = 0; i < scr_width * scr_lines * 2; i++, ptr++)
	{
		*ptr = 0;
		ptr++;
		*ptr = 0;
	}
}

int vt_print_char(char ch, char attr, int r, int c)
{
	int i, j;
	char *ptr;

	ptr = video_mem;
	ptr = ptr + r * 2 * scr_width + 2 * c;

	*ptr = ch;
	ptr++;
	*ptr = attr;
	return 0;
}

int vt_print_string(char *str, char attr, int r, int c)
{
	char *ptr;

	ptr = video_mem;
	ptr = ptr + r * 2 * scr_width + 2 * c;

	while (*str != '\0')
	{
		*ptr = *str;
		ptr++;
		str++;
		*ptr = attr;
		ptr++;
	}
	return 0;
}

int vt_print_int(int num, char attr, int r, int c)
{
	char *ptr, *str;
	int i = num;

	ptr = video_mem;
	ptr = ptr + r * 2 * scr_width + 2 * c;

	while (i > 0)
	{
		str++;
		i = i / 10;
	}

	*str = '\0';

	while (num > 0)
	{
		str--;
		*str = (char) (num % 10 + (int) '0');
		num = num / 10;
	}

	while (*str != '\0')
	{
		*ptr = *str;
		ptr++;
		str++;
		*ptr = attr;
		ptr++;
	}
	return 0;
}


int vt_draw_frame(int width, int height, char attr, int r, int c)
{
	char *ptr;
	int w = 0;
	int h = 1;

	ptr = video_mem;
	ptr = ptr + r * 2 * scr_width + 2 * c;

	// aresta superior
	while (w < width)
	{
		*ptr = 0;
		ptr++;
		*ptr = attr;
		ptr++;
		w++;
	}

	// aresta lateral direita
	while (h < height)
	{
		ptr = ptr + 2 * (scr_width - width - 1) + 2 * width;
		*ptr = 0;
		ptr++;
		*ptr = attr;
		ptr++;
		h++;
	}

	// aresta lateral esquerda
	ptr = video_mem;
	ptr = ptr + r * 2 * scr_width + 2 * c + 2;
	h = 1;
	while (h < height)
	{
		ptr = ptr + 2 * (scr_width - 1);
		*ptr = 0;
		ptr++;
		*ptr = attr;
		ptr++;
		h++;
	}

	// aresta inferior
	w = 1;
	while (w < width)
	{
		*ptr = 0;
		ptr++;
		*ptr = attr;
		ptr++;
		w++;
	}
	return 0;
}

/*
 * THIS FUNCTION IS FINALIZED, do NOT touch it
 */

char *vt_init(vt_info_t *vi_p) {

  int r;
  struct mem_range mr;

  /* Allow memory mapping */

  mr.mr_base = (phys_bytes)(vi_p->vram_base);
  mr.mr_limit = mr.mr_base + vi_p->vram_size;

  if( OK != (r = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr)))
	  panic("video_txt: sys_privctl (ADD_MEM) failed: %d\n", r);

  /* Map memory */

  video_mem = vm_map_phys(SELF, (void *)mr.mr_base, vi_p->vram_size);

  if(video_mem == MAP_FAILED)
	  panic("video_txt couldn't map video memory");

  /* Save text mode resolution */

  scr_lines = vi_p->scr_lines;
  scr_width = vi_p->scr_width;

  return video_mem;
}
