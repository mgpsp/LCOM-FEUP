#ifndef _BOMB_H
#define _BOMB_H

#include "Bitmap.h"

/** @defgroup bomb bomb
 * @{
 *
 * Functions to manipulate the bomb
 */

/*! Represents a bomb */
typedef struct
{
	int x; /*!< x coordinate of the bomb */
	int y; /*!< y coordinate of the bomb */
	int width; /*!< width of the bitmap of the bomb */
	int height; /*!< height of the bitmap of the bomb */
	int velocity; /*!< velocity of the bomb */
	int collisions; /*!< number of buildings that the bomb collided width */

	Bitmap* sprite; /*!< pointer to the bitmap of the bomb */
} Bomb;

/**
 * @brief Creates a new bomb
 *
 * @return Returns a pointer to the new bomb created
 */
Bomb* newBomb();

/**
 * @brief Updates the given bomb
 *
 * Increments the velocity by GRAVITY and the y coordinate by bomb velocity.
 *
 * @param bomb Bomb to update
 */
void updateBomb(Bomb* bomb);

/**
 * @brief Draws the given bomb
 *
 * @param bomb Bomb to draw
 */
void drawBomb(Bomb* bomb);

/**
 * @brief Destroys the given bomb, freeing all resources used by it
 *
 * @param bomb Bomb to be destroyed
 */
void deleteBomb(Bomb* bomb);

/**@}*/

#endif /* _BOMB_H */
