#include <minix/syslib.h>
#include <minix/drivers.h>

#include "timer.h"
#include "i8254.h"

unsigned int counter = 0;
int id = 0;

int timer_set_square(unsigned long timer, unsigned long freq)
{
	int status;
	unsigned long lsb, msb, cMode, oMode;
	unsigned char conf;

	timer_get_conf(timer, &conf);
	cMode = conf>>1;
	oMode = conf>>3;
	status = sys_outb(TIMER_CTRL, (TIMER_0 + timer | TIMER_LSB_MSB | oMode | cMode));

	if (status != OK) return 1;

	unsigned long f = TIMER_FREQ / freq;
	lsb = f;
	msb = f>>8;
	status =  sys_outb(TIMER_0 + timer, lsb);
	status =  sys_outb(TIMER_0 + timer, msb);

	if (status != OK) return 1;

	return 0;
}

int timer_subscribe_int(void)
{
	int status, tmp = id;

	status = sys_irqsetpolicy(TIMER0_IRQ, IRQ_REENABLE, &id);
	if (status != OK) return -1;

	status = sys_irqenable(&id);
	if (status != OK) return -1;

	return BIT(tmp);
}

int timer_unsubscribe_int()
{
	int status;

	status = sys_irqrmpolicy(&id);
	if (status != OK) return 1;

	status = sys_irqdisable(&id);
	if (status != OK) return 1;

	return 0;
}

void timer_int_handler()
{
	counter++;
}

int timer_get_conf(unsigned long timer, unsigned char *st)
{
	int status;
	unsigned long rbcmd, byte;

	rbcmd = (TIMER_RB_CMD | TIMER_RB_COUNT_ | TIMER_RB_SEL(timer));

	status = sys_outb(TIMER_CTRL, rbcmd);

	if (status != OK) return 1;

	status = sys_inb(TIMER_0 + timer, &byte);

	*st = (unsigned char) byte;

	if (status != OK) return 1;

	return 0;
}

int timer_display_conf(unsigned char conf)
{
	unsigned char dup;

	printf("Counting mode: ");
	if (conf & BIT(0)) printf("BCD\n");
	else printf("Binary\n");

	conf >>= 1;
	dup = conf;
	dup =  dup & (BIT(0) | BIT(1) | BIT(2));
	printf("Operating mode: %u\n", dup);

	conf >>= 3;
	dup = conf;
	dup =  dup & (BIT(0) | BIT(1));
	printf("Type of access: ");
	if (dup == 0) printf("LSB\n");
	else if (dup == 1) printf("MSB\n");
	else printf("LSB followed by MSB\n");

	conf >>= 2;
	printf("Counter: %u\n", conf);

	return 0;
}

int timer_test_square(unsigned long freq)
{
	timer_set_square(0, freq);
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
				{
					timer_int_handler();
					if ((i % 60 == 0) || (i == 0)) printf("This is a message.\n");
				}
				break;
			default:
				break;
			}
		}
		i++;
	}
	
	timer_unsubscribe_int();

	return 0;
}

int timer_test_config(unsigned long timer)
{
	unsigned char conf;
	if(timer < 0 || timer > 2) return 1;
	timer_get_conf(timer, &conf);
	timer_display_conf(conf);
	return 0;
}
