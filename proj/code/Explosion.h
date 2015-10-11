#ifndef _EXPLOSION_H
#define _EXPLOSION_H

#include "Bitmap.h"

/** @defgroup explosion explosion
 * @{
 *
 * Functions to manipulate the explosions
 */

/*! Represents an explosion */
typedef struct
{
	int x; /*!< x coordinate of the explosion */
	int y; /*!< y coordinate of the explosion */
	int time; /*!< time that the explosion is on screen, in number of timer interruptions */
} Explosion;

/**
 * @brief Gets the pointer to the bmp image of the explosion
 *
 * @return Returns the pointer to the bmp image of the explosion
 */
Bitmap* getExplosionBitmap();

/**
 * @brief Gets a reference of the pointer to the bmp image of the explosion
 *
 * @return Returns the reference of the pointer to the bmp image of the explosion
 */
Bitmap** getExplosionBitmapPointer();

/**
 * @brief Creates a new explosion at the given position
 *
 * @param x x coordinate of the explosion
 * @param y y coordinate of the explosion
 * @return Returns a pointer to the new explosion created
 */
Explosion* newExplosion(int x, int y);

/**
 * @brief Draws the given explosion
 *
 * @param explosion Explosion to draw
 */
void drawExplosion(Explosion* explosion);

/**
 * @brief Destroys the given explosion, freeing all resources used by it
 *
 * @param explosion Explosion to be destroyed
 */
void deleteExplosion(Explosion* explosion);

/**@}*/

#endif /* _EXPLOSION_H */
