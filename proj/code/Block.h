#ifndef _BLOCK_H
#define _BLOCK_H

#include "Bitmap.h"

/** @defgroup block block
 * @{
 *
 * Functions to manipulate the blocks
 */

/*! Represents a block */
typedef struct
{
	int x; /*!< x coordinate of the block */
	int y; /*!< y coordinate of the block */
} Block;

/**
 * @brief Gets the pointer to the bmp image of the block
 *
 * @return Returns the pointer to the bmp image of the block
 */
Bitmap* getBlockBitmap();

/**
 * @brief Gets a reference of the pointer to the bmp image of the block
 *
 * @return Returns the reference of the pointer to the bmp image of the block
 */
Bitmap** getBlockBitmapPointer();

/**
 * @brief Creates a new block at the given position
 *
 * @param x x coordinate of the block
 * @param y y coordinate of the block
 * @return Returns a pointer to the new block created
 */
Block* newBlock(int x, int y);

/**
 * @brief Draws the given block
 *
 * @param block Block to draw
 */
void drawBlock(Block* block);

/**
 * @brief Destroys the given block, freeing all resources used by it
 *
 * @param block Block to be destroyed
 */
void deleteBlock(Block* block);

/**@}*/

#endif /* _BLOCK_H */
