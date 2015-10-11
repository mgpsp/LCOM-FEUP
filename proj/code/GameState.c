#include <stdlib.h>
#include <time.h>

#include "GameState.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "Graphics.h"

GameState* newGameState()
{
	GameState* state = (GameState*) malloc(sizeof(GameState));

	state->done = 0;
	state->pause = 0;
	state->score = 0;

	state->ground = loadBitmap("/home/images/ground.bmp");
	state->header = loadBitmap("/home/images/header.bmp");
	state->pausedGame = loadBitmap("/home/images/pausedGame.bmp");

	state->chopper = newChopper();
	state->bomb = newBomb();

	state->dropBomb = 0;

	// Randomly generate buildings
	int i, height;
	int buildingHeights[7] = {0};

	for (i = 0; i < NUM_BUILDINGS; i++)
	{
		do
		{
			height = rand() % MAX_HEIGHT + 1;
		} while (buildingHeights[height] == 4);
		buildingHeights[height]++;

		state->buildings[i] = newBuilding(i, height);
	}

	// Initialize explosions to NULL
	for (i = 0; i < NUM_EXPLOSIONS; i++)
		state->explosions[i] = NULL;

	state->action = PLAYING;

	return state;
}

void createNewExplosion(GameState* state, int x, int y)
{
	int i = 0;

	// Find array position for the new explosion
	while (i < NUM_EXPLOSIONS && state->explosions[i] != NULL)
		i++;

	if (i < NUM_EXPLOSIONS)
		state->explosions[i] = newExplosion(x, y);
}

void detectBombCollisions(GameState* state)
{
	// Calculate where to check for collisions
	int building1Index = (state->bomb->x - STARTING_XPOSITION) / (BLOCK_WIDTH + 4);
	int building2Index = (state->bomb->x + state->bomb->width - STARTING_XPOSITION) / (BLOCK_WIDTH + 4);

	// Last building exception
	if (building2Index == NUM_BUILDINGS)
		building2Index = NUM_BUILDINGS - 1;

	// Check if the index is valid
	if (building1Index < 0 || building1Index > NUM_BUILDINGS - 1 || building2Index < 0 || building2Index > NUM_BUILDINGS - 1)
		return;

	int buildingIndex;
	if (building1Index == building2Index)
		buildingIndex = building1Index;
	else
	{
		// Bomb may be colliding with two different buildings, check which is higher
		if (state->buildings[building2Index]->height > state->buildings[building1Index]->height)
			buildingIndex = building2Index;
		else
			buildingIndex = building1Index;
	}

	int height = state->buildings[buildingIndex]->height;

	// No need to check for collisions if the bomb hasn't reached the buildings or has already made all possible collisions
	if (state->bomb->y + state->bomb->height < vres - (height * BLOCK_HEIGHT + STARTING_YPOSITION) || state->bomb->collisions == MAX_COLLISIONS)
		return;


	// Collision
	if (height != 0)
	{
		Block* block = state->buildings[buildingIndex]->blocks[height - 1];

		//Create new explosion
		createNewExplosion(state, block->x, block->y);

		// Delete block
		deleteBlock(block);

		// Decrement building height
		state->buildings[buildingIndex]->height--;

		// Increment bomb collisions and score
		state->bomb->collisions++;
		state->score++;
	}
}

int gameOver(GameState* state)
{
	// Find building the chopper may be colliding with
	int buildingIndex = (state->chopper->x + state->chopper->width - STARTING_XPOSITION) / (BLOCK_WIDTH + 4);

	// Check if the chopper is on the screen
	if (buildingIndex < 0 || buildingIndex > 18)
		return 0;

	int height = state->buildings[buildingIndex]->height;

	// Check if the chopper has collided with the building
	if (state->chopper->y + state->chopper->height < vres - (height * BLOCK_HEIGHT + STARTING_YPOSITION))
		return 0;

	return 1;
}

int gameWon(GameState* state)
{
	int i = 0;

	// Game is won when all buildings are destructed
	while (i < NUM_BUILDINGS && state->buildings[i]->height == 0)
		i++;

	return (i == NUM_BUILDINGS);
}

void shiftExplosions(GameState* state, int start)
{
	int i;

	for (i = start + 1; i < NUM_EXPLOSIONS; i++)
		state->explosions[i - 1] = state->explosions[i];
}

void updateExplosions(GameState* state)
{
	int i = 0;

	while (state->explosions[i] != NULL)
	{
		if (state->explosions[i]->time == TIME_ON_SCREEN)
		{
			deleteExplosion(state->explosions[i]);
			shiftExplosions(state, i);
		}
		else
			state->explosions[i]->time++;

		i++;
	}
}

void updateGameState(GameState* state, unsigned long scancode, int timerCounter)
{
	// Quit if ESC has been pressed or pause the game if P has been pressed
	if (scancode == KEY_DOWN(KEY_ESC) && !state->pause)
	{
		state->done = 1;
		state->action = END_GAME;
	}
	else if (scancode == KEY_UP(KEY_P))
		state->pause = 1;

	// Don't update positions if the game is paused
	if (state->pause && scancode != KEY_UP(KEY_ESC))
		return;
	else
		state->pause = 0;

	if (state->action == GAME_OVER)
		updateChopper(state->chopper, 1);
	else
		updateChopper(state->chopper, 0);

	// Drop bomb if the mouse's left button or the space bar has been pressed,
	if (getMouse()->leftButtonReleased || scancode == KEY_UP(KEY_SPACE))
	{
		// But only if there are no bombs on the screen and if at least half of the chopper on the screen
		if (!state->dropBomb
				&& state->chopper->x > - (state->chopper->width / 2) && state->chopper->x < hres - (state->chopper->width / 2) - 20)
		{
			// Draw a bomb that leaves from approximately the middle of the chopper
			state->bomb->x = state->chopper->x + (state->chopper->width / 2) + 7;
			state->bomb->y = state->chopper->y + state->chopper->height;
			state->dropBomb = 1;

		}

		getMouse()->leftButtonReleased = 0;
	}

	// Check if the bomb has reached the end of the screen
	if (state->bomb->y > vres)
	{
		state->dropBomb = 0;
		state->bomb->velocity = 0;
		state->bomb->collisions = 0;
	}

	// Update bomb at 20 FPS
	if (timerCounter % 3 == 0 && state->dropBomb)
		updateBomb(state->bomb);

	// Check bomb colissions
	if (state->dropBomb)
		detectBombCollisions(state);

	// Update explosions
	updateExplosions(state);

	if (gameOver(state))
		state->action = GAME_OVER;


	if (state->action == GAME_OVER)
	{
		// Wait for game over animation to finish
		if (state->chopper->y + state->chopper->height >= vres - STARTING_YPOSITION || state->chopper->x >= hres)
			state->done = 1;
	}

	if (gameWon(state))
	{
		state->action = GAME_WON;

		// Wait for bomb to completly drop
		if (state->bomb->y > vres)
			state->done = 1;
	}

}

void drawBuildings(GameState* state)
{
	int i;

	for (i = 0; i < NUM_BUILDINGS; i++)
		drawBuilding(state->buildings[i]);
}

void drawExplosions(GameState* state)
{
	int i = 0;

	while (state->explosions[i] != NULL)
	{
		drawExplosion(state->explosions[i]);
		i++;
	}
}

void drawGameState(GameState* state, int timerCounter)
{
	if (state->pause)
	{
		drawBitmap(state->pausedGame, 0, 0, ALIGN_LEFT);
		return;
	}

	// Draw background
	fillDisplay(BLACK);
	drawBitmap(state->header, 0, 0, ALIGN_LEFT);
	drawBitmap(state->ground, 0, vres - 10, ALIGN_LEFT);

	// Draw score
	drawNumber(state->score, hres - 37, 14);

	// Draw Buildings
	drawBuildings(state);

	// Draw explosions
	drawExplosions(state);

	// Draw Chopper
	drawChopper(state->chopper, timerCounter);

	// Draw bomb if necessary
	if (state->dropBomb)
		drawBomb(state->bomb);
}

void deleteBuildings(GameState* state)
{
	int i;

	for (i = 0; i < NUM_BUILDINGS; i++)
		deleteBuilding(state->buildings[i]);
}

void deleteExplosions(GameState* state)
{
	int i;

	for(i = 0; i < NUM_EXPLOSIONS; i++)
		deleteExplosion(state->explosions[i]);
}

void deleteGameState(GameState* state)
{
	deleteBitmap(state->header);
	deleteBitmap(state->ground);
	deleteBitmap(state->pausedGame);
	deleteChopper(state->chopper);
	deleteBomb(state->bomb);
	deleteBuildings(state);
	deleteExplosions(state);

	deleteBitmap(getBlockBitmap());
	*getBlockBitmapPointer() = NULL;

	deleteBitmap(getExplosionBitmap());
	*getExplosionBitmapPointer() = NULL;

	free(state);
}
