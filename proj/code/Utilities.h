#ifndef _UTILITIES_H
#define _UTILITIES_H

#include "Block.h"
#include "Bitmap.h"

/** @defgroup utilities utilities
 * @{
 *
 * Useful functions and constants
 */

#define BIT(n) 			(0x01<<(n))

int hres; /*!< horizontal screen resolution in pixels */
int vres; /*!< vertical screen resolution in pixels */
int bytesPerPixel; /*!< number of VRAM bytes per pixel */

// Colors
#define RED 		0xF800
#define BLUE		0x001F
#define GREEN 		0x07E0
#define YELLOW		0xFFE0
#define WHITE 		0xFFFF
#define BLACK		0x0000
#define DARK_GRAY	0x18E3

#define STARTING_HEIGHT		70 /*!< Starting y coordinate of the chopper */
#define GRAVITY				2
#define CHOPPER_VELOCITY	5
#define CHOPPER_DROP		35 /*!< Increment the y coordinate of the chopper by this value every time the chopper completes the an horizontal movement */

#define NUM_BUILDINGS		19 /*!< Number of buildings to create */
#define MAX_HEIGHT			6 /*!< Maximum number of blocks of a building */
#define STARTING_XPOSITION	60 /*!< The first building is drawn in this x coordinate */
#define STARTING_YPOSITION	14 /*!< The first building is drawn in this y coordinate */
#define BUILDING_DISTANCE	4
#define BLOCK_DISTANCE		1
#define BLOCK_WIDTH			getBlockBitmap()->bitmapInfoHeader.width
#define BLOCK_HEIGHT		getBlockBitmap()->bitmapInfoHeader.height

#define TIME_ON_SCREEN		30  /*!< Time that the explosion is on the screen, in number of timer interrupts */
#define NUM_EXPLOSIONS		10 /*!< Maximum number of simultaneous explosions*/

#define MAX_COLLISIONS		4 /*!< Maximum number of collisions for a single bomb */

// Text
Bitmap* digits[10]; /*!< array of pointers to the bitmaps of the digits 0-9  */
Bitmap* slash; /*!< pointer to the bitmap of the slash */
Bitmap* colon; /*!< pointer to the bitmap of the colon */

/**
 * @brief Draws a number at the given position
 *
 * @param number Number to draw
 * @param x x coordinate of the rightmost digit of the number
 * @param y y coordinate of the rightmost digit of the number
 * @return Returns the x coordinate of the leftmost digit
 */
int drawNumber(int number, int x, int y);

/**@}*/

#endif /* _UTILITIES_H */
