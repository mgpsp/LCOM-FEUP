#ifndef _BUILDING_H
#define _BUILDING_H

#include "Block.h"
#include "Utilities.h"

/** @defgroup building building
 * @{
 *
 * Functions to manipulate the buildings
 */

/*! Represents a building */
typedef struct
{
	int height; /*!< height of the building in number of blocks */

	Block* blocks[MAX_HEIGHT];  /*!< array of pointers to the blocks that compose the building */
} Building;

/**
 * @brief Creates a new building with the given height
 *
 * @param buildingIndex Index of the building in the array of buildings in the GameState struct
 * @param height Height of the building to be created
 * @return Returns a pointer to the new building created
 */
Building* newBuilding(int buildingIndex, int height);

/**
 * @brief Draws the given building
 *
 * @param building Building to draw
 */
void drawBuilding(Building* building);

/**
 * @brief Destroys the given building, freeing all resources used by it
 *
 * @param building Building to be destroyed
 */
void deleteBuilding(Building* building);

/**@}*/

#endif /* _BUILDING_H */
