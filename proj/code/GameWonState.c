#include "GameWonState.h"
#include "Keyboard.h"

GameWonState* newGameWonState()
{
	GameWonState* state = (GameWonState*) malloc(sizeof(GameWonState));

	state->done = 0;

	state->background = loadBitmap("/home/images/gameWon.bmp");

	return state;
}

void updateGameWonState(GameWonState* state, unsigned long scancode)
{
	if (scancode == KEY_UP(KEY_ENTER))
		state->done = 1;
}

void drawGameWonState(GameWonState* state)
{
	drawBitmap(state->background, 0, 0, ALIGN_LEFT);
}

void deleteGameWonState(GameWonState* state)
{
	deleteBitmap(state->background);
	free(state);
}
