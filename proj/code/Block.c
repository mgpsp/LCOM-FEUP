#include <stdlib.h>

#include "Block.h"

Bitmap* blockBitmap = NULL;

Bitmap* getBlockBitmap()
{
	if (!blockBitmap)
		blockBitmap = loadBitmap("/home/images/block.bmp");

	return blockBitmap;
}

Bitmap** getBlockBitmapPointer()
{
	return &blockBitmap;
}

Block* newBlock(int x, int y)
{
	Block* block = (Block*) malloc(sizeof(Block));

	block->x = x;
	block->y = y;

	return block;
}

void drawBlock(Block* block)
{
	drawBitmap(getBlockBitmap(), block->x, block->y, ALIGN_LEFT);
}

void deleteBlock(Block* block)
{
	free(block);
}
