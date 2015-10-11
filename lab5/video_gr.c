#include <minix/syslib.h>
#include <minix/drivers.h>
#include <machine/int86.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/video.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "vbe.h"
#include "timer.h"
#include "video_gr.h"
#include "keyboard.h"
#include "sprite.h"
#include "read_xpm.h"

/* Constants for VBE 0x105 mode */

/* The physical address may vary from VM to VM.
 * At one time it was 0xD0000000
 *  #define VRAM_PHYS_ADDR    0xD0000000 
 * Currently on lab B107 is 0xF0000000
 * Better run my version of lab5 as follows:
 *     service run `pwd`/lab5 -args "mode 0x105"
 */

////////////////

#define VRAM_PHYS_ADDR			0xF0000000
#define H_RES             		1024
#define V_RES		  			768
#define BITS_PER_PIXEL	  		8

#define BIT(n) 					(0x01<<(n))
#define VIDEO_CARD				0x10
#define SET_VBE_MODE			0x4F02
#define LINEAR_MODEL_BIT 		14

#define ESC				0x81

////////////////

/* Private global variables */

static char *video_mem;		/* Process address to which VRAM is mapped */

static unsigned h_res;		/* Horizontal screen resolution in pixels */
static unsigned v_res;		/* Vertical screen resolution in pixels */
static unsigned bits_per_pixel; /* Number of VRAM bits per pixel */
static unsigned bytes_per_pixel;

int vg_exit() {
  struct reg86u reg86;

  reg86.u.b.intno = 0x10; /* BIOS video services */

  reg86.u.b.ah = 0x00;    /* Set Video Mode function */
  reg86.u.b.al = 0x03;    /* 80x25 text mode*/

  if( sys_int86(&reg86) != OK ) {
      printf("\tvg_exit(): sys_int86() failed \n");
      return 1;
  } else
      return 0;
}

void *vg_init(unsigned short mode)
{
	struct reg86u reg86;
	vbe_mode_info_t vmi_p;
	struct mem_range mr;
	unsigned long vram_size;

	reg86.u.w.ax = SET_VBE_MODE;
	reg86.u.w.bx = BIT(LINEAR_MODEL_BIT)|mode;
	reg86.u.b.intno = VIDEO_CARD;

	if (sys_int86(&reg86) != OK)
	{
		printf("\tvg_init(): sys_int86() failed \n");
		return NULL;
	}

	if (vbe_get_mode_info(mode, &vmi_p) != 0)
	{
		printf("\tvg_init(): vbe_get_mode_info() failed \n");
		return NULL;
	}

	// Set resolution

	h_res = vmi_p.XResolution;
	v_res = vmi_p.YResolution;
	bits_per_pixel = vmi_p.BitsPerPixel;

	if (bits_per_pixel/8 <= 0)
		bytes_per_pixel = 1;
	else
		bytes_per_pixel = bits_per_pixel/8;

	vram_size = h_res*v_res*bytes_per_pixel;

	// Allow memory mapping

	mr.mr_base = (phys_bytes)(vmi_p.PhysBasePtr);
	mr.mr_limit = mr.mr_base + vram_size;

	if (sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr) != OK)
	{
		printf("\tvg_init(): sys_privctl() failed \n");
		return NULL;
	}

	 // Map memory

	video_mem = vm_map_phys(SELF, (void *)mr.mr_base, vram_size);

	if (video_mem == MAP_FAILED)
	{
		printf("\tvg_init(): video_gr couldn't map video memory \n");
		return NULL;
	}

	 return video_mem;
}

int vg_square(unsigned short x, unsigned short y, unsigned short size, unsigned long color)
{
	int i, j;
	char *ptr;

	// Set graphics mode
	vg_init(0x105);

	ptr = video_mem;

	// Draw square

	for (i = 0; i < size; i++)
	{
		ptr += (y + i) * bytes_per_pixel * h_res + x * bytes_per_pixel;
		for (j = 0; j < size; j++)
		{
			*ptr = (char) color;
			ptr += bytes_per_pixel;
		}
		ptr = video_mem;
	}

	if (exit_on_ESC() != 0)
	{
		printf("\tvg_square(): exit_on_ESC() failed\n");
		return 1;
	}

	return 0;
}

void draw_pixel(unsigned short x, unsigned short y, unsigned long color, char *ptr)
{
	if (x < 0 || x > h_res || y < 0 || y > v_res)
		return;

	ptr += y * bytes_per_pixel * h_res + x * bytes_per_pixel;
	*ptr = (char) color;
}

int vg_line(unsigned short xi, unsigned short yi,  unsigned short xf, unsigned short yf, unsigned long color)
{
	int i, j, tmp;
	float m, b;
	char *ptr;

	// Set graphics mode
	vg_init(0x105);

	ptr = video_mem;

	// Draw line

	if (xi < 0 || yi < 0 || xf < 0 || yf < 0)
		return 0;

	if (xf < xi)
	{
		tmp = xf;
		xf = xi;
		xi =  tmp;
	}
	if (yf < yi)
	{
		tmp = yf;
		yf = yi;
		yi = tmp;
	}

	if (xi > h_res)
		xi = h_res;
	if (xf > h_res)
		xf = h_res;
	if (yf > v_res)
		yf = v_res;
	if (yi > v_res)
		yi = v_res;

	m = (float) (yf-yi)/(xf-xi);
	b = yi - m*xi;

	for (i = xi, j = yi; i < xf && j < yf; i++)
	{
		j = m*i + b;
		draw_pixel(i, j, color, ptr);
	}


	if (exit_on_ESC() != 0)
	{
		printf("\tvg_square(): exit_on_ESC() failed\n");
		return 1;
	}

	return 0;
}

int vg_xpm(unsigned short xi, unsigned short yi, char *xpm[])
{
	char *ptr, *pix;
	int wd, ht, i, j, k = 0;

	// Set graphics mode
	vg_init(0x105);

	ptr = video_mem;

	// Draw picture
	pix = read_xpm(xpm, &wd, &ht);

	for (i = 0; i < ht; i++)
	{
		ptr += (yi + i) * bytes_per_pixel * h_res + xi * bytes_per_pixel;
		for (j = 0; j < wd; j++)
		{
			*ptr = (char) pix[k];
			ptr += bytes_per_pixel;
			k++;
		}
		ptr = video_mem;
	}

	if (exit_on_ESC() != 0)
	{
		printf("\tvg_square(): exit_on_ESC() failed\n");
		return 1;
	}

	return 0;
}

int clear_screen()
{
	int i;
	char *ptr;
	unsigned long color = 0;

	ptr = video_mem;

	for (i = 0; i < v_res * h_res; i++)
	{
		*ptr = (char) color;
		ptr += bytes_per_pixel;
	}

	return 0;
}

void draw_sprite(Sprite *sp, unsigned short xf, unsigned short yf)
{
	int i, j, k = 0;
	char *ptr;
	float x, y;

	ptr = video_mem;

	x = sp->x;
	y = sp->y;
	x += 0.5;
	x =  (int) x;
	y += 0.5;
	y =  (int) y;

	if (x < xf || y < yf)
	{
		clear_screen();
		for (i = 0; i < sp->height; i++)
		{
			for (j = 0; j < sp->width; j++)
			{
				if (x > 0 && x+sp->width < h_res && y > 0 && y+sp->height < v_res)
				{
					draw_pixel(x, (y + i), sp->map[k], ptr);
					ptr += bytes_per_pixel;
					k++;
				}
			}
			ptr = video_mem;
		}
		sp->x += sp->xspeed;
		sp->y += sp->yspeed;
		k = 0;
	}
}

int vg_move(unsigned short xi, unsigned short yi, char *xpm[], unsigned short hor, short delta, unsigned short time)
{
	char *ptr, *pix;
	unsigned short xf, yf;
	int i, j, k, ipc_status, r, exit = 0;
	float tmp;
	unsigned long keyboard_set, timer_set, data;
	message msg;

	// Set graphics mode
	vg_init(0x105);
	ptr = video_mem;

	// Create sprite
	Sprite *sp = create_sprite(xpm, video_mem);

	// Set initial position
	if (xi > 1024 || yi > 768 || xi < 0 || yi < 0)
		return 0;
	sp->x = xi;
	sp->y = yi;

	// Set speed
	if (hor == 0)
	{
		sp->yspeed = (float) delta / (float) (time * 60);
		sp->xspeed = 0;
		yf = yi + delta;
		xf = xi;
		if (yf > 768)
			yf = 768;
	}
	else
	{
		sp->xspeed = (float) delta / (float) (time * 60);
		sp->yspeed = 0;
		xf = xi + delta;
		yf = yi;
		if (xf > 1024)
			xf = 1024;
	}

	// Make movement
	keyboard_set = keyboard_subscribe_int();
	timer_set = timer_subscribe_int();

	if (timer_set == -1 || keyboard_set == -1)
	{
		vg_exit();
		return 1;
	}

	do
	{
		r = driver_receive(ANY, &msg, &ipc_status);
		if (r != 0)
		{
			printf("driver_receive failed with: %d", r);
			continue;
		}

		if (is_ipc_notify(ipc_status))
		{
			switch (_ENDPOINT_P(msg.m_source))
			{
			case HARDWARE:

				if (msg.NOTIFY_ARG & keyboard_set)
					data = readFromKBC();

				if (msg.NOTIFY_ARG & timer_set)
					draw_sprite(sp, xf, yf);

				break;
			default:
				break;
			}
		}
	} while (data != ESC);
	keyboard_unsubscribe_int();
	timer_unsubscribe_int();

	destroy_sprite(sp, video_mem);

	if (vg_exit() != 0)
	{
		printf("\texit_on_ESC(): vg_exit() failed\n");
		return 1;
	}

	return 0;
}

