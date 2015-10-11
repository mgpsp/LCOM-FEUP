#include <minix/drivers.h>
#include <stdbool.h>
#include <math.h>

#include "test4.h"
#include "i8042.h"

int mouse_id = 0, timer0_id = 1, counter = 0, xpos = 0, ypos = 0;
int packet[3];

int subscribe_int(int irq_vec, int policy, int *hook_id)
{
	int status, tmp = *hook_id;

	// IRQ_EXCLUSIVE is needed to prevent minix from calling the default IH
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

unsigned long readFromKBC()
{
	unsigned long stat, r, data, i = 0;

	while(i < 5) // retry 3 times on time-out
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

int writeToMouse(unsigned long cmd)
{
	int status;

	writeToKBC(STAT_REG, WRITE_BYTE_TO_MOUSE);
	if (status == -1)
	{
		printf("writeToKBC(WRITE_BYTE_TO_MOUSE) failed\n");
		return -1;
	}

	writeToKBC(OUT_BUF, cmd);
	if (status == -1)
	{
		printf("writeToKBC(SET_SMODE) failed\n");
		return -1;
	}
	return 0;
}

int disable_stream_mode()
{
	int status;
	unsigned long response;

	status = writeToMouse(DISABLE_SMODE);
	if (status == -1)
	{
		printf("writeToMouse(DISABLE_SMODE) failed\n");
		return -1;
	}

	status = sys_inb(OUT_BUF, &response);
	if (status != OK)
	{
		printf("sys_inb failed\n");
		return -1;
	}

	if (response != ACK)
		printf("Acknowledgment byte is not ACK\n");

	return 0;
}

int enable_stream_mode()
{
	int status;
	unsigned long response;

	status = writeToMouse(SET_SMODE);
	if (status == -1)
	{
		printf("writeToMouse(SET_SMODE) failed\n");
		return -1;
	}

	status = sys_inb(OUT_BUF, &response);
	if (status != OK)
	{
		printf("sys_inb failed\n");
		return -1;
	}
	if (response != ACK)
		printf("Acknowledgment byte is not ACK\n");

	return 0;
}

int enable_data_packets()
{
	int status;
	unsigned long response;

	disable_stream_mode();

	status = writeToMouse(ENABLE_SENDING_DPACKETS);
	if (status == -1)
	{
		printf("writeToMouse(ENABLE_SENDING_DPACKETS) failed\n");
		return -1;
	}

	status = sys_inb(OUT_BUF, &response);
	if (status != OK)
	{
		printf("sys_inb failed\n");
		return -1;
	}

	if (response != ACK)
		printf("Acknowledgment byte is not ACK\n");

	enable_stream_mode();
	return 0;
}

void print_packet()
{
	int LB, MB, RB, XOV, YOV, X, Y, byte2, byte3;
	int convert = BIT(7) | BIT(6) | BIT(5) | BIT(4) | BIT(3) | BIT(2) | BIT(1) | BIT(0);

	if (packet[0] & BIT(0))
		LB = 1;
	else
		LB = 0;

	if (packet[0] & BIT(1))
		RB = 1;
	else
		RB = 0;

	if (packet[0] & BIT(2))
		MB = 1;
	else
		MB = 0;

	if (packet[0] & BIT(6))
		XOV = 1;
	else
		XOV = 0;

	if (packet[0] & BIT(7))
		YOV = 1;
	else
		YOV = 0;

	printf("B1=0x%X B2=0x%X B3=0x%X   LB=%d   MB=%d   RB=%d   XOV=%d   YOV=%d", packet[0], packet[1], packet[2], LB, MB, RB, XOV, YOV);

	// convert from twos complement
	byte2 = packet[1];
	byte3 = packet[2];

	printf("   X=");
	if (byte2 & BIT(7))
	{
		byte2 = byte2 ^ convert;
		byte2++;
		printf("-%d", byte2);
	}
	else
		printf("%d", byte2);

	printf("   Y=");
	if (byte3 & BIT(7))
	{
		byte3 = byte3 ^ convert;
		byte3++;
		printf("-%d\n", byte3);
	}
	else
		printf("%d\n", byte3);
}

bool get_packet()
{
	unsigned long byte = readFromKBC();
	bool complete;

	if (byte == -1)
		return false;

	switch (counter)
	{
	case 0:
		if (byte & BIT(3))
			packet[counter] = byte;
		counter++;
		complete = false;
		break;
	case 1:
		packet[counter] = byte;
		counter++;
		complete = false;
		break;
	case 2:
		packet[counter] = byte;
		counter = 0;
		complete = true;
		break;
	}

	return complete;
}

int test_packet(unsigned short cnt)
{
	int ipc_status, r, i = 0, flag = 0;
	unsigned long irq_set, data;
	message msg;

	irq_set = subscribe_int(MOUSE_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &mouse_id);
	enable_data_packets();

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
					if (get_packet())
						print_packet();
					i++;
				}
				break;
			default:
				break;
			}
		}
	} while (i < (cnt * 3));
	printf("Done\n");

	disable_stream_mode();
	unsubscribe_int(&mouse_id);

	return 0;
}	
	
int test_async(unsigned short idle_time)
{
	int ipc_status, r, exit = 0, max, c = 0, flag = 0;
	unsigned long irq1, irq2, data;
	message msg;

	irq2 = subscribe_int(TIMER0_IRQ, IRQ_REENABLE, &timer0_id);
	irq1 = subscribe_int(MOUSE_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &mouse_id);
	enable_data_packets();
	if (irq1 == -1 || irq2 == -1) return -1;  // check if subscribe_int failed

	max = idle_time*60;
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
					c = 0;
					if (get_packet())
						print_packet();
				}
				if (msg.NOTIFY_ARG & irq2)
				{
					c++;
					if (c >= max)
						exit = 1;
				}
				break;
			default:
				break;
			}
		}
	} while (exit == 0);
	printf("Done\n");

	disable_stream_mode();
	unsubscribe_int(&mouse_id);
	unsubscribe_int(&timer0_id);

	return 0;
}
	
int test_config(void)
{
	int status;
	unsigned long irq_set, response, BYTE_1, BYTE_2, BYTE_3;

	irq_set = subscribe_int(MOUSE_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &mouse_id);

	disable_stream_mode();

	writeToMouse(STATUS_REQUEST);

	status = sys_inb(OUT_BUF, &response);
	if (status != OK)
	{
		printf("sys_inb failed\n");
		return -1;
	}
	if (response != ACK)
		printf("Acknowledgment byte is not ACK\n");
	else
	{
		status = sys_inb(OUT_BUF, &BYTE_1);
		if (status != OK)
		{
			printf("sys_inb failed - BYTE_1\n");
			return -1;
		}
		status = sys_inb(OUT_BUF, &BYTE_2);
		if (status != OK)
		{
			printf("sys_inb failed - BYTE_2\n");
			return -1;
		}
		status = sys_inb(OUT_BUF, &BYTE_3);
		if (status != OK)
		{
			printf("sys_inb failed - BYTE_3\n");
			return -1;
		}

		if (BYTE_1 & BIT(6))
			printf("Remote mode\n");
		else
		{
			printf("Stream mode\n");
			if (BYTE_1 & BIT(5))
				printf("Data reporting enabled\n");
			else
				printf("Data reporting disabled\n");
		}

		if (BYTE_1 & BIT(4))
			printf("Scaling is 2:1\n");
		else
			printf("Scaling is 1:1\n");

		if (BYTE_1 & BIT(2))
			printf("Left button is currently pressed\n");
		else
			printf("Left button is currently released\n");

		if (BYTE_1 & BIT(1))
			printf("Middle button is currently pressed\n");
		else
			printf("Middle button is currently released\n");

		if (BYTE_1 & BIT(0))
			printf("Right button is currently pressed\n");
		else
			printf("Right button is currently released\n");

		printf("Resolution: ");
		switch(BYTE_2)
		{
		case 0:
			printf("1 count/mm\n");
			break;
		case 1:
			printf("2 count/mm\n");
			break;
		case 2:
			printf("4 count/mm\n");
			break;
		case 3:
			printf("8 count/mm\n");
			break;
		}

		printf("Sample rate: %d\n", BYTE_3);
	}

	unsubscribe_int(&mouse_id);

	return 0;
}	

int check_hor_line(short length, unsigned short tolerance)
{
	int byte2, byte3, convert = BIT(7) | BIT(6) | BIT(5) | BIT(4) | BIT(3) | BIT(2) | BIT(1) | BIT(0);
	byte2 = packet[1];
	byte3 = packet[2];

	if (packet[0] & BIT(0))  // left button is pressed
	{
		// check if the movement was in the direction of length
		if (byte2 & BIT(7))
		{
			if (length < 0)
			{
				byte2 = byte2 ^ convert;
				byte2++;
				xpos += byte2;
			}
			else
				xpos = 0;
		}
		else if (length > 0)
			xpos += byte2;
		else
			xpos = 0;

		// check tolerance
		if (byte3 & BIT(7))
		{
			byte3 = byte3 ^ convert;
			byte3++;
			ypos -= byte3;
		}
		else
			ypos += byte3;
	}
	else
	{
		xpos = 0;
		ypos = 0;
	}

	if (abs(ypos) >= tolerance)
		xpos = 0;

	if (xpos >= abs(length))
		return 1;
	else return 0;
}

int test_gesture(short length, unsigned short tolerance)
{
	int ipc_status, r, i = 0, flag = 1, exit = 0;
	unsigned long irq_set, data;
	message msg;

	enable_data_packets();
	irq_set = subscribe_int(MOUSE_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &mouse_id);

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
					if (get_packet())
						exit = check_hor_line(length, tolerance);
				}
				break;
			default:
				break;
			}
		}
	} while (exit == 0);
	printf("Done\n");

	xpos = 0;
	ypos = 0;

	disable_stream_mode();
	unsubscribe_int(&mouse_id);

	return 0;
}
