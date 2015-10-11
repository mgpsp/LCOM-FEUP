#include <minix/drivers.h>

#include "RTC.h"

static int rtc_id = 4;

Date* newDate()
{
	Date* date = (Date*) malloc(sizeof(Date));

	date->second = 0;
	date->minute = 0;
	date->hour = 0;
	date->month = 0;
	date->year = 0;
	date->monthDay = 0;

	return date;
}

unsigned char BCDtoBinary(unsigned char bcd)
{
	char buffer[32];
	unsigned char binary;

	sprintf(buffer, "%x", bcd);
	sscanf(buffer, "%d", &binary);

	return binary;
}

void updateDate(Date* date)
{
	unsigned long seconds, minutes, hours, mday, month, year, regC, regA;

	do
	{
		sys_outb(RTC_ADDR_REG, REG_A);
		sys_inb(RTC_DATA_REG, &regA);
		regA >>= 7;
	} while (regA & UIP);

	// Clear flags
	sys_outb(RTC_ADDR_REG, REG_C);
	sys_inb(RTC_DATA_REG, &regC);

	// Read date
	sys_outb(RTC_ADDR_REG, SECONDS);
	sys_inb(RTC_DATA_REG, &seconds);

	sys_outb(RTC_ADDR_REG, MINUTES);
	sys_inb(RTC_DATA_REG, &minutes);

	sys_outb(RTC_ADDR_REG, HOURS);
	sys_inb(RTC_DATA_REG, &hours);

	sys_outb(RTC_ADDR_REG, MONTH_DAY);
	sys_inb(RTC_DATA_REG, &mday);

	sys_outb(RTC_ADDR_REG, MONTH);
	sys_inb(RTC_DATA_REG, &month);

	sys_outb(RTC_ADDR_REG, YEAR);
	sys_inb(RTC_DATA_REG, &year);

	// Convert to binary and save
	date->second = BCDtoBinary(seconds);
	date->minute = BCDtoBinary(minutes);
	date->hour = BCDtoBinary(hours);
	date->month = BCDtoBinary(month);
	date->year = 20*100 + BCDtoBinary(year);
	date->monthDay = BCDtoBinary(mday);
}

void drawDate(Date* date)
{
	int x = hres - 13;
	int y = vres - 13 - 23;

	if (date->monthDay == 0)
		return;

	x = drawNumber(date->year, x, y);
	x -= slash->bitmapInfoHeader.width + 2;
	drawBitmap(slash, x, y, ALIGN_LEFT);
	x = drawNumber(date->month, x - 2, y);
	if (date->month < 10)
		x = drawNumber(0, x - 2, y);
	x -= slash->bitmapInfoHeader.width + 2;
	drawBitmap(slash, x, y, ALIGN_LEFT);
	x = drawNumber(date->monthDay, x - 2, y);
	if (date->monthDay < 10)
		x = drawNumber(0, x - 2, y);

	x -= 14;
	x = drawNumber(date->second, x, y);
	if (date->second < 10)
		x = drawNumber(0, x - 2, y);
	x -= colon->bitmapInfoHeader.width + 2;
	drawBitmap(colon, x, y, ALIGN_LEFT);
	x = drawNumber(date->minute, x - 2, y);
	if (date->minute < 10)
		x = drawNumber(0, x - 2, y);
	x -= colon->bitmapInfoHeader.width + 2;
	drawBitmap(colon, x, y, ALIGN_LEFT);
	x = drawNumber(date->hour, x - 2, y);
	if (date->hour < 10)
		x = drawNumber(0, x - 2, y);
}

void deleteDate(Date* date)
{
	free(date);
}

int subscribeRTC()
{
	//Configure RTC to generate update interrupts
	unsigned long reg = BIT(4) | BIT(1);

	if (sys_outb(RTC_ADDR_REG, REG_B) != OK)
	{
		printf("\tsubscribeRTC() in RTC.c: sys_outb failed");
		return 1;
	}

	if (sys_outb(RTC_DATA_REG, reg) != OK)
	{
		printf("\tsubscribeRTC() in RTC.c: sys_outb failed");
		return 1;
	}

	if (sys_irqsetpolicy(RTC_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &rtc_id) != OK)
	{
		printf("\tsubscribeRTC() in RTC.c: sys_irqsetpolicy failed");
		return -1;
	}

	if (sys_irqenable(&rtc_id) != OK)
	{
		printf("\tsubscribeRTC() in RTC.c: sys_irqenable failed");
		return -1;
	}

	return BIT(4);
}

int unsubscribeRTC()
{
	if (sys_irqdisable(&rtc_id) != OK)
	{
		printf("\tunsubscribeRTC() in RTC.c: sys_irqdisable failed");
		return 1;
	}

	if (sys_irqrmpolicy(&rtc_id) != OK)
	{
		printf("\tunsubscribeRTC() in RTC.c: sys_irqrmpolicy failed");
		return 1;
	}

	return 0;
}
