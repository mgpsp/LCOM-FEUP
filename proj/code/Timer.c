#include <minix/syslib.h>
#include <minix/drivers.h>

#include "Timer.h"
#include "i8254.h"
#include "Graphics.h"

static int timer_id = 1;

Timer* newTimer()
{
	Timer* timer = (Timer*) malloc(sizeof(Timer));

	timer->ticked = 0;
	timer->counter = 0;

	return timer;
}

void deleteTimer(Timer* timer)
{
	free(timer);
}

void resetTickedFlag(Timer* timer)
{
	timer->ticked = 0;
}

void timerHandler(Timer* timer)
{
	timer->counter++;
	timer->ticked = 1;
}

int subscribeTimer()
{
	if (sys_irqsetpolicy(TIMER0_IRQ, IRQ_REENABLE, &timer_id) != OK)
	{
		printf("\ttimer_subscribe_int() in Timer.c: sys_irqsetpolicy failed");
		return -1;
	}

	if (sys_irqenable(&timer_id) != OK)
	{
		printf("\ttimer_subscribe_int() in Timer.c: sys_irqenable failed");
		return -1;
	}

	return BIT(1);
}

int unsubscribeTimer()
{
	if (sys_irqdisable(&timer_id) != OK)
	{
		printf("\ttimer_unsubscribe_int() in Timer.c:  sys_irqdisable failed");
		return 1;
	}

	if (sys_irqrmpolicy(&timer_id) != OK)
	{
		printf("\ttimer_unsubscribe_int() in Timer.c: sys_irqrmpolicy failed");
		return 1;
	}

	return 0;
}
