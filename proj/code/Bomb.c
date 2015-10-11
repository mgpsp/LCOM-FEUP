#include "Bomb.h"
#include "Utilities.h"

Bomb* newBomb()
{
	Bomb* bomb = (Bomb*) malloc(sizeof(Bomb));

	bomb->sprite = loadBitmap("/home/images/bomb.bmp");

	bomb->width = bomb->sprite->bitmapInfoHeader.width;
	bomb->height = bomb->sprite->bitmapInfoHeader.height;
	bomb->x = 0;
	bomb->y = 0;

	bomb->velocity = 0;
	bomb->collisions = 0;

	return bomb;
}

void updateBomb(Bomb* bomb)
{
	bomb->velocity += GRAVITY;
	bomb->y += bomb->velocity;
}

void drawBomb(Bomb* bomb)
{
	drawBitmap(bomb->sprite, bomb->x, bomb->y, ALIGN_LEFT);
}

void deleteBomb(Bomb* bomb)
{
	deleteBitmap(bomb->sprite);
	free(bomb);
}
