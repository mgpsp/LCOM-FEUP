#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/sysutil.h>

#include "Keyboard.h"
#include "Utilities.h"

static int kb_id = 0;

int subscribeKeyboard()
{
	// IRQ_EXCLUSIVE is needed to prevent minix from printing the character in the terminal
	if (sys_irqsetpolicy(KB_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &kb_id) != OK)
	{
		printf("\tsubscribeKeyboard() in Keyboard.c: sys_irqsetpolicy failed");
		return -1;
	}

	if (sys_irqenable(&kb_id) != OK)
	{
		printf("\tsubscribeKeyboard() in Keyboard.c: sys_irqenable failed");
		return -1;
	}

	return BIT(0);
}

int unsubscribeKeyboard()
{
	if (sys_irqdisable(&kb_id) != OK)
	{
		printf("\tsubscribeKeyboard() in Keyboard.c: sys_irqdisable failed");
		return 1;
	}

	if (sys_irqrmpolicy(&kb_id) != OK)
	{
		printf("\tsubscribeKeyboard() in Keyboard.c: sys_irqrmpolicy failed");
		return 1;
	}

	return 0;
}
