#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/sysutil.h>

#include "test3.h"
#include "i8042.h"

unsigned long asm_readFromKBC();

int kb_id = 0, timer0_id = 1;

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

int writeToKBC(unsigned long reg, unsigned long cmd)
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

		if ((stat & IBF) == 0)
		{
			r = sys_outb(reg, cmd);
			if (r != OK)
			{
				printf("sys_inb failed with: %d", r);
				return -1;
			}
			else return 0;
		}

		// gives the KBC or the keyboard enough-time to respond
		tickdelay(micros_to_ticks(DELAY_US));

		i++;
	}

	printf("writeToKBC failed: unable to write data\n");
	return -1;
}

unsigned long keyboard_handler(unsigned short ass)
{
	unsigned long data;
	if (ass == 0) data = readFromKBC();
	else data = asm_readFromKBC();
	return data;
}

int subscribe_int(int irq_vec, int policy, int *hook_id)
{
	int status, tmp = *hook_id;

	// IRQ_EXCLUSIVE is needed to prevent minix from printing the character in the terminal
	status = sys_irqsetpolicy(irq_vec, policy, hook_id);
	if (status != OK)
	{
		printf("sys_irqsetpolicy failed with: %d", status);
		return -1;
	}

	status = sys_irqenable(hook_id);
	if (status != OK)
	{
		printf("sys_irqenable failed with: %d", status);
		return -1;
	}

	return BIT(tmp);
}

int unsubscribe_int(int *hook_id)
{
	int status;

	status = sys_irqdisable(hook_id);
	if (status != OK)
	{
		printf("sys_irqdisable failed with: %d", status);
		return -1;
	}

	status = sys_irqrmpolicy(hook_id);
	if (status != OK)
	{
		printf("sys_irqrmpolicy failed with: %d", status);
		return -1;
	}

	return 0;
}

int print_scancodes(unsigned long data, int flag)
{
	int k = 0;

	if (data == -1)
	{
		printf("Invalid data\n");
		return -1;
	}

	// printing two-byte long scancodes
	if (data == 0xE0)
	{
		flag = 1;
		return flag;
	}

	if (flag == 1)
	{
		if (BREAKCODE & data)
			printf("Breakcode: 0xE0%02X\n", data);
		else
			printf("Makecode: 0xE0%02X\n", data);
		flag = 0;
		k = 1;
	}

	// one-byte long scancode
	if (k == 0)
	{
		if (BREAKCODE & data)
			printf("Breakcode: 0x%02X\n", data);
		else
			printf("Makecode: 0x%02X\n", data);
	}
	return flag;
}

int kbd_test_scan(unsigned short ass)
{
	int ipc_status, r, exit = 0, flag = 0;
	unsigned long irq_set, data;
	message msg;

	irq_set = subscribe_int(KB_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &kb_id);
	if (irq_set == -1) return -1;  // check if subscribe_int failed

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
					data = keyboard_handler(ass);
					flag = print_scancodes(data, flag);
					if (data == ESC)
					{
						printf("Done\n");
						exit = 1;
					}
				}
				break;
			default:
				break;
			}
		}
	} while (exit == 0);

	unsubscribe_int(&kb_id);

	return 0;
}

int change_leds(unsigned long optionByte)
{
	unsigned long data;

	// command byte
	writeToKBC(OUT_BUF, KBC_CMD_ED);
	sys_inb(OUT_BUF, &data);

	if (data == KB_ACK)
		writeToKBC(OUT_BUF, optionByte);
	else
	{
		printf("Set/Reset command failed\n");
	    return -1;
	}
	return 0;
}

int kbd_test_leds(unsigned short n, unsigned short *leds)
{
	int ipc_status, i = 0, it, r, k = 0;
	unsigned long irq_set, cmd, sLock = 0, cLock = 0, nLock = 0;
	message msg;

	it = n * 60;

	irq_set = subscribe_int(TIMER0_IRQ, IRQ_REENABLE, &timer0_id);
	if (irq_set == -1) return 1; // check if subscribe_int failed

	while(i < it)
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
					if ((i % 60 == 0) || (i == 0)) // toggle led after 1 second
					{
						switch(leds[k])
						{
						case 0:
							if (sLock == 0) sLock = BIT(0);
							else sLock = 0;
							break;
						case 1:
							if (nLock == 0) nLock = BIT(1);
							else nLock = 0;
							break;
						case 2:
							if (cLock == 0) cLock = BIT(2);
							else cLock = 0;
							break;
						default:
							printf("Not a valid indicator\n");
							return 1;
							break;
						}
						// option byte
						cmd = sLock | nLock | cLock;
						change_leds(cmd);
						k++;
					}
				}
				break;
			default:
				break;
			}
		}
		i++;
	}

	unsubscribe_int(&timer0_id);

    return 0;
}

int kbd_test_timed_scan(unsigned short n)
{
	int ipc_status, r, exit = 0, max, counter = 0, flag = 0;
	unsigned long irq1, irq2, data;
	message msg;

	irq2 = subscribe_int(TIMER0_IRQ, IRQ_REENABLE, &timer0_id);
	irq1 = subscribe_int(KB_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &kb_id);
	if (irq1 == -1 || irq2 == -1) return -1;  // check if subscribe_int failed

	max = n*60;

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
				if (msg.NOTIFY_ARG & irq1)
				{
					counter = 0;
					data = keyboard_handler(0);
					flag = print_scancodes(data, flag);
					if (data == ESC)
					{
						printf("Done\n");
						exit = 1;
					}
				}
				if (msg.NOTIFY_ARG & irq2)
				{
					counter++;
					if (counter >= max)
					{
						printf("Done\n");
						exit = 1;
					}
				}
				break;
			default:
				break;
			}
		}
	} while (exit == 0);

	unsubscribe_int(&kb_id);
	unsubscribe_int(&timer0_id);

	return 0;
}
