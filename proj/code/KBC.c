#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/sysutil.h>

#include "KBC.h"
#include "Utilities.h"

unsigned long readFromKBC(int ass)
{
	unsigned long stat, r, data, i = 0;

	if (ass == 0)
	{
		data = readFromKBC_asm();
		return  data;
	}

	while(i < 3) // retry 3 times on time-out
	{
		r = sys_inb(STAT_REG, &stat);
		if (r != OK)
		{
			printf("\treadFromKBC() in KBC.c: sys_inb failed with: %d", r);
			return -1;
		}

		if (stat & OBF)
		{
			r = sys_inb(OUT_BUF, &data);
			if (r != OK)
			{
				printf("\treadFromKBC() in KBC.c: sys_inb failed with: %d", r);
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

	return -1;
}

int writeToKBC(unsigned long reg, unsigned long cmd)
{
	unsigned long stat, r, data, i = 0;

	while (i < 3) // retry 3 times on time-out
	{
		r = sys_inb(STAT_REG, &stat);
		if (r != OK)
		{
			printf("\twriteToKBC() in KBC.c: sys_inb failed with: %d", r);
			return -1;
		}

		if ((stat & IBF) == 0)
		{
			r = sys_outb(reg, cmd);
			if (r != OK)
			{
				printf("\twriteToKBC() in KBC.c: sys_inb failed with: %d", r);
				return -1;
			}
			else
				return 0;
		}

		// gives the KBC or the keyboard enough-time to respond
		tickdelay(micros_to_ticks(DELAY_US));

		i++;
	}

	printf("\twriteToKBC() in KBC.c: unable to write data\n");

	return -1;
}

int cleanBuffer()
{
	unsigned long stat, data;

	sys_inb(STAT_REG, &stat);
	while (stat & OBF)
	{
		sys_inb(OUT_BUF, &data);
		sys_inb(STAT_REG, &stat);
	}

	return 0;
}
