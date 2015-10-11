#include <minix/drivers.h>

#include "Mouse.h"
#include "KBC.h"
#include "Utilities.h"
#include "Graphics.h"

static int mouse_id = 3;

Mouse* mouse = NULL;

Mouse* newMouse()
{
	Mouse* mouse = (Mouse*) malloc(sizeof(Mouse));

	mouse->x = 0;
	mouse->y = 0;

	mouse->packetRead = 0;
	mouse->counter = 0;

	mouse->leftButtonDown = 0;
	mouse->rightButtonDown = 0;

	mouse->leftButtonReleased = 0;
	mouse->rightButtonReleased = 0;

	mouse->draw = 1;
}

Mouse* getMouse()
{
	if (!mouse)
	{
		enableMouse();
		mouse = newMouse();
	}

	return mouse;
}

void updateMouse()
{
	unsigned long byte = readFromKBC(1);

	if (byte == -1)  // unable to read from KBC
		return;

	switch (mouse->counter)
	{
	case 0:
		if (byte & BIT(3))
		{
			mouse->packet[mouse->counter] = byte;
			mouse->counter++;
		}
		mouse->packetRead = 0;
		break;
	case 1:
		mouse->packet[mouse->counter] = byte;
		mouse->counter++;
		mouse->packetRead = 0;
		break;
	case 2:
		mouse->packet[mouse->counter] = byte;
		mouse->counter = 0;
		mouse->packetRead = 1;
		break;
	}

	if (mouse->packetRead)
	{
		unsigned long byte2, byte3;

		// update mouse position
		byte2 = mouse->packet[1];
		byte3 = mouse->packet[2];

		if (mouse->packet[0] & BIT(4))
		{
			byte2 = byte2 ^ (BIT(7) | BIT(6) | BIT(5) | BIT(4) | BIT(3) | BIT(2) | BIT(1) | BIT(0));
			byte2++;
			mouse->x -= byte2;
		}
		else
			mouse->x += byte2;

		if (mouse->packet[0] & BIT(5))
		{
			byte3 = byte3 ^ (BIT(7) | BIT(6) | BIT(5) | BIT(4) | BIT(3) | BIT(2) | BIT(1) | BIT(0));
			byte3++;
			mouse->y += byte3;
		}
		else
			mouse->y -= byte3;

		if (mouse->x >= hres)
			mouse->x = hres - 1;
		else if (mouse->x < 0)
			mouse->x = 0;

		if (mouse->y >= vres)
			mouse->y = vres - 2;
		else if (mouse->y < 0)
			mouse->y = 0;


		// update buttons state
		if (mouse->packet[0] & BIT(0))
		{
			mouse->leftButtonDown = 1;
			mouse->leftButtonReleased = 0;
		}
		else if (mouse->leftButtonDown)
		{
			mouse->leftButtonReleased = 1;
			mouse->leftButtonDown = 0;
		}
		else
		{
			mouse->leftButtonDown = 0;
			mouse->leftButtonReleased = 0;
		}

		if (mouse->packet[0] & BIT(1))
		{
			mouse->rightButtonDown = 1;
			mouse->rightButtonReleased = 0;
		}
		else if (mouse->rightButtonDown)
		{
			mouse->rightButtonReleased = 1;
			mouse->rightButtonDown = 0;
		}
		else
		{
			mouse->rightButtonDown = 0;
			mouse->rightButtonReleased = 0;
		}

		mouse->draw = 1;
	}
	else
		mouse->draw = 0;
}

void deleteMouse()
{
	free(getMouse());
}

int mouseInside(int x1, int x2, int y1, int y2)
{
	return (getMouse()->x >= x1 && getMouse()->x <= x2 && getMouse()->y >= y1 && getMouse()->y <= y2);
}

int writeToMouse(unsigned long command)
{
	if (writeToKBC(STAT_REG, WRITE_BYTE_TO_MOUSE))
	{
		printf("\twriteToMouse() in Mouse.c: writeToKBC() failed\n");
		return 1;
	}

	if (writeToKBC(OUT_BUF, command))
	{
		printf("\twriteToMouse() in Mouse.c: writeToKBC() failed\n");
		return 1;
	}

	return 0;
}

int enableMouse()
{
	unsigned long response;

	// Enable data packets
	if (writeToMouse(ENABLE_SENDING_DPACKETS))
	{
		printf("\tenableMouse() in Mouse.c: writeToMouse() failed\n");
		return 1;
	}

	if (sys_inb(OUT_BUF, &response) != OK)
	{
		printf("\tenableMouse() in Mouse.c: sys_inb failed\n");
		return 1;
	}

	if (response != ACK)
	{
		printf("enableMouse() in Mouse.c: acknowledgment byte is not ACK\n");
		return 1;
	}

	// Enable stream mode
	if (writeToMouse(SET_STREAM_MODE))
	{
		printf("\tenableMouse() in Mouse.c: writeToMouse() failed\n");
		return 1;
	}

	if (sys_inb(OUT_BUF, &response) != OK)
	{
		printf("\tenableMouse() in Mouse.c: sys_inb failed\n");
		return 1;
	}

	if (response != ACK)
	{
		printf("enableMouse() in Mouse.c: acknowledgment byte is not ACK\n");
		return 1;
	}

	return 0;
}

int disableMouse()
{
	unsigned long response;

	if (writeToMouse(DISABLE_STREAM_MODE))
	{
		printf("\tdisableMouse() in Mouse.c: writeToMouse() failed\n");
		return 1;
	}

	if (sys_inb(OUT_BUF, &response) != OK)
	{
		printf("\tdisableMouse() in Mouse.c: sys_inb failed\n");
		return 1;
	}

	if (response != ACK)
	{
		printf("disableMouse() in Mouse.c: acknowledgment byte is not ACK\n");
		return 1;
	}

	return 0;
}

int subscribeMouse()
{
	if (sys_irqsetpolicy(MOUSE_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &mouse_id) != OK)
	{
		printf("\tsubscribeMouse() in Mouse.c: sys_irqsetpolicy failed");
		return -1;
	}

	if (sys_irqenable(&mouse_id) != OK)
	{
		printf("\tsubscribeMouse() in Mouse.c: sys_irqenable failed");
		return -1;
	}

	return BIT(3);
}

int unsubscribeMouse()
{
	if (sys_irqdisable(&mouse_id) != OK)
	{
		printf("\tunsubscribeMouse() in Mouse.c: sys_irqdisable failed");
		return 1;
	}

	if (sys_irqrmpolicy(&mouse_id) != OK)
	{
		printf("\tunsubscribeMouse() in Mouse.c: sys_irqrmpolicy failed");
		return 1;
	}

	return 0;
}
