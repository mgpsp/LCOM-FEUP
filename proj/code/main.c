#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <minix/drivers.h>

#include "ChopperDrop.h"
#include "Graphics.h"
#include "Mouse.h"

int main(int argc, char **argv)
{
	/* Initialize service */
	sef_startup();

	sys_enable_iop(SELF);

	srand(time(NULL));

	initializeGraphics(0x114);

	ChopperDrop* game = (ChopperDrop*) initializeChopperDrop();

	updateDate(game->date);
	drawDate(game->date);

	while(!game->done)
	{
		updateChopperDrop(game);

		if (!game->done)
		{
			int flag = 0;

			if (game->draw)
			{
				flag = 1;
				drawChopperDrop(game);
			}

			if (getMouse()->draw)
			{
				flag = 1;
				drawMouse(game);
			}

			if (flag)
				swapBuffer();
		}
	}

	stopChopperDrop(game);

	exitGraphics();

	return 0;
}
