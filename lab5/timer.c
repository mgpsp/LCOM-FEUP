#include <minix/syslib.h>
#include <minix/drivers.h>

#include "timer.h"

////////////////

#define BIT(n) 			(0x01<<(n))
#define TIMER0_IRQ	    0

////////////////

int timer_id = 1;

int timer_subscribe_int()
{
	int status, tmp = timer_id;

	status = sys_irqsetpolicy(TIMER0_IRQ, IRQ_REENABLE, &timer_id);
	if (status != OK) return -1;

	status = sys_irqenable(&timer_id);
	if (status != OK) return -1;

	return BIT(tmp);
}

int timer_unsubscribe_int()
{
	int status;

	status = sys_irqdisable(&timer_id);
	if (status != OK) return 1;

	status = sys_irqrmpolicy(&timer_id);
	if (status != OK) return 1;

	return 0;
}


int timer_test_int(unsigned long time)
{
	int ipc_status, i = 0, it, r;
	unsigned long irq_set;
	message msg;

	it = time * 60;

	irq_set = timer_subscribe_int();

	if (irq_set == -1) return 1;

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
					i++;
				break;
			default:
				break;
			}
		}
	}

	timer_unsubscribe_int();

	return 0;
}
