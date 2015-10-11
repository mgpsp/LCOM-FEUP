#include <stdlib.h>

#include "Building.h"

Building* newBuilding(int buildingIndex, int height)
{
	Building* building = (Building*) malloc(sizeof(Building));

	building->height = height;

	// Construct building
	int i;

	for (i = 0; i < MAX_HEIGHT; i++)
		building->blocks[i] = NULL;

	for (i = 0; i < height; i++)
		building->blocks[i] = newBlock(buildingIndex * (BLOCK_WIDTH + BUILDING_DISTANCE) + STARTING_XPOSITION, vres - (BLOCK_HEIGHT * (i + 1) + STARTING_YPOSITION + i * BLOCK_DISTANCE));

	return building;
}

void drawBuilding(Building* building)
{
	int i;

	for (i = 0; i < building->height; i++)
		drawBlock(building->blocks[i]);
}

void deleteBuilding(Building* building)
{
	int i;

	for (i = 0; i < building->height; i++)
		deleteBlock(building->blocks[i]);
}
