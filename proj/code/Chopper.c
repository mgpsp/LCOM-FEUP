#include "Chopper.h"
#include "Utilities.h"

Chopper* newChopper()
{
	Chopper* chopper = (Chopper*) malloc(sizeof(Chopper));

	chopper->spriteRight = loadBitmap("/home/lcom/proj/code/images/chopperRight.bmp");
	chopper->spriteLeft = loadBitmap("/home/lcom/proj/code/images/chopperLeft.bmp");

	chopper->width = chopper->spriteLeft->bitmapInfoHeader.width;
	chopper->height = chopper->spriteLeft->bitmapInfoHeader.height;
	chopper->x = - chopper->width - 50;
	chopper->y = STARTING_HEIGHT;

	return chopper;
}

void updateChopper(Chopper* chopper, int gameOver)
{
	if (!gameOver)
	{
		if (chopper->x < (hres + 200))
			chopper->x += CHOPPER_VELOCITY;
		else
		{
			chopper->y += CHOPPER_DROP;
			chopper->x = -chopper->width - 50;
		}
	}
	else
	{
		// Make game over animation
		if (chopper->y + chopper->height < vres - STARTING_YPOSITION)
		{
			chopper->x += CHOPPER_VELOCITY;
			chopper->y += 2;
		}
	}

}

void drawChopper(Chopper* chopper, int timerCounter)
{
	if (timerCounter % 2 == 0)
		drawBitmap(chopper->spriteRight, chopper->x, chopper->y, ALIGN_LEFT);
	else
		drawBitmap(chopper->spriteLeft, chopper->x, chopper->y, ALIGN_LEFT);
}

void deleteChopper(Chopper* chopper)
{
	deleteBitmap(chopper->spriteRight);
	deleteBitmap(chopper->spriteLeft);
	free(chopper);
}
