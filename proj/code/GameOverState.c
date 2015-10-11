#include "GameOverState.h"
#include "Keyboard.h"

GameOverState* newGameOverState()
{
	GameOverState* state = (GameOverState*) malloc(sizeof(GameOverState));

	state->done = 0;

	state->background = loadBitmap("/home/images/gameOver.bmp");

	return state;
}

void updateGameOverState(GameOverState* state, unsigned long scancode)
{
	if (scancode == KEY_UP(KEY_ENTER))
		state->done = 1;
}

void drawGameOverState(GameOverState* state)
{
	drawBitmap(state->background, 0, 0, ALIGN_LEFT);
}

void deleteGameOverState(GameOverState* state)
{
	deleteBitmap(state->background);
	free(state);
}
