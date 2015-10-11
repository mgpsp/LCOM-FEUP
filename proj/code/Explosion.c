#include <stdlib.h>

#include "Explosion.h"
#include "Utilities.h"

Bitmap* explosionBitmap = NULL;

Bitmap* getExplosionBitmap()
{
	if (!explosionBitmap)
		explosionBitmap = loadBitmap("/home/images/explosion.bmp");

	return explosionBitmap;
}

Bitmap** getExplosionBitmapPointer()
{
	return &explosionBitmap;
}

Explosion* newExplosion(int x, int y)
{
	Explosion* explosion = (Explosion*) malloc(sizeof(Explosion));

	explosion->x = x;
	explosion->y = y;
	explosion->time = 0;

	return explosion;
}

void drawExplosion(Explosion* explosion)
{
	if (!explosion)
		return;

	drawBitmap(getExplosionBitmap(), explosion->x, explosion->y, ALIGN_LEFT);
}

void deleteExplosion(Explosion* explosion)
{
	free(explosion);
}
