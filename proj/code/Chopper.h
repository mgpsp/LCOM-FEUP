#ifndef _CHOPPER_H
#define _CHOPPER_H

#include "Bitmap.h"

/** @defgroup chopper chopper
 * @{
 *
 * Functions to manipulate the chopper
 */

/*! Represents the chopper */
typedef struct
{
	int x; /*!< x coordinate of the chopper */
	int y; /*!< x coordinate of the chopper */
	int width; /*!< width of the bitmap of the chopper */
	int height; /*!< height of the bitmap of the chopper */

	Bitmap* spriteLeft; /*!< pointer to the bitmap of the chopper with the left rotor blade */
	Bitmap* spriteRight; /*!< pointer to the bitmap of the chopper with the right rotor blade */
} Chopper;

/**
 * @brief Creates a new chopper
 *
 * The x and y coordinates of the chooper are STARTING_HEIGHT and "- chopper->width - 50" respectively
 *
 * @return Returns a pointer to the new chopper created
 */
Chopper* newChopper();

/**
 * @brief Updates the given chopper
 *
 * The x coordinate is incremented by CHOPPER_VELOCITY; once the chopper completes the horizontal movement the y coordinate is incremented by CHOPPER_DROP.
 * If the chopper has collided with a building, the x and y coordinates increment until the chopper is off screen or has hit the ground.
 *
 * @param chopper Chopper to draw
 * @param gameOver If 1, the chopper has collided with a building
 */
void updateChopper(Chopper* chopper, int gameOver);

/**
 * @brief Draws the given chopper
 *
 * Draws the the left and right sprites alternately.
 *
 * @param chopper Chopper to draw
 * @param timerCounter Timer counter
 */
void drawChopper(Chopper* chopper, int timerCounter);

/**
 * @brief Destroys the given chopper, freeing all resources used by it
 *
 * @param chopper Chopper to be destroyed
 */
void deleteChopper(Chopper* chopper);

/**@}*/

#endif /* _CHOPPER_H */
