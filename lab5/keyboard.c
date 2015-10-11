#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/sysutil.h>

#include "keyboard.h"
#include "video_gr.h"

////////////////

#define BIT(n) 			(0x01<<(n))
#define ESC				0x81
#define STAT_REG		0x64
#define OUT_BUF 		0x60
#define TO_ERR 			BIT(6)
#define PAR_ERR 		BIT(7)
#define DELAY_US 		20000
#define OBF 			BIT(0)
#define KB_IRQ			1

////////////////

int kb_id = 0;

unsigned long readFromKBC()
{
	unsigned long stat, r, data, i = 0;

	while(i < 3) // retry 3 times on time-out
	{
		r = sys_inb(STAT_REG, &stat);
		if (r != OK)
		{
			printf("sys_inb failed with: %d", r);
			return -1;
		}

		if (stat & OBF)
		{
			r = sys_inb(OUT_BUF, &data);
			if (r != OK)
			{
				printf("sys_inb failed with: %d", r);
				return -1;
			}

			if ((stat & (PAR_ERR | TO_ERR)) == 0)
				return data;
			else
				return -1;
		}

		// gives the KBC or the keyboard enough-time to respond
		tickdelay(micros_to_ticks(DELAY_US));

		i++;
	}

	printf("readFromKBC failed: no data to read\n");
	return -1;
}

int keyboard_subscribe_int()
{
	int status, tmp = kb_id;

	// IRQ_EXCLUSIVE is needed to prevent minix from printing the character in the terminal
	status = sys_irqsetpolicy(KB_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &kb_id);
	if (status != OK)
	{
		printf("sys_irqsetpolicy failed with: %d", status);
		return -1;
	}

	status = sys_irqenable(&kb_id);
	if (status != OK)
	{
		printf("sys_irqenable failed with: %d", status);
		return -1;
	}

	return BIT(tmp);
}

int keyboard_unsubscribe_int()
{
	int status;

	status = sys_irqdisable(&kb_id);
	if (status != OK)
	{
		printf("sys_irqdisable failed with: %d", status);
		return -1;
	}

	status = sys_irqrmpolicy(&kb_id);
	if (status != OK)
	{
		printf("sys_irqrmpolicy failed with: %d", status);
		return -1;
	}

	return 0;
}

int exit_on_ESC()
{
	int ipc_status, r, exit = 0;
	unsigned long irq_set, data;
	message msg;

	irq_set = keyboard_subscribe_int();
	if (irq_set == -1) return 1;  // check if subscribe_int failed

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
				if (msg.NOTIFY_ARG & irq_set)
				{
					data = readFromKBC();
					if (data == ESC)
					{
						exit = 1;
						if (vg_exit() != 0)
						{
							printf("\texit_on_ESC(): vg_exit() failed\n");
							return 1;
						}
					}
				}
				break;
			default:
				break;
			}
		}
	} while (exit == 0);

	keyboard_unsubscribe_int();

	return 0;
}
