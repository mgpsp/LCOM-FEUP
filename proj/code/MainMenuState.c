#include "MainMenuState.h"
#include "Keyboard.h"
#include "Graphics.h"
#include "Mouse.h"
#include "Utilities.h"

Rectangle* newRectangle(int x, int y, int width, int height)
{
	Rectangle* rectangle = (Rectangle*) malloc(sizeof(Rectangle));

	rectangle->x = x;
	rectangle->y = y;
	rectangle->width = width;
	rectangle->height = height;

	return rectangle;
}

MainMenuState* newMainMenuState()
{
    MainMenuState* state = (MainMenuState*) malloc(sizeof(MainMenuState));

    state->done = 0;

    state->background = loadBitmap("/home/images/mainMenu.bmp");
    state->playHover = loadBitmap("/home/images/playHover.bmp");
    state->exitHover = loadBitmap("/home/images/exitHover.bmp");

    state->mouseOnPlay = 0;
    state->playButton = newRectangle(413, 311, 189, 61);
    state->mouseOnExit = 0;
    state->exitButton = newRectangle(422, 397, 161, 61);

    return state;
}

void updateMainMenuState(MainMenuState* state, unsigned long scancode)
{
	// Quit if ESC has been pressed
	if (scancode == KEY_DOWN(KEY_ESC))
	{
		state->done = 1;
		state->action = EXIT_GAME;
	}


	// Check if the mouse is on the play button
	int x1, x2, y1, y2;
	x1 = state->playButton->x;
	x2 = state->playButton->x + state->playButton->width;
	y1 = state->playButton->y;
	y2 = state->playButton->y + state->playButton->height;

	if (mouseInside(x1, x2, y1, y2))
	{
		state->mouseOnPlay = 1;

		if (getMouse()->leftButtonReleased)
		{
			state->action = PLAY_GAME;
			state->done = 1;
			getMouse()->leftButtonReleased = 0;
		}
	}

	else
		state->mouseOnPlay = 0;

	// Check if the mouse is on the exit button
	x1 = state->exitButton->x;
	x2 = state->exitButton->x + state->exitButton->width;
	y1 = state->exitButton->y;
	y2 = state->exitButton->y + state->exitButton->height;

	if (mouseInside(x1, x2, y1, y2))
	{
		state->mouseOnExit = 1;

		if (getMouse()->leftButtonReleased)
		{
			state->action = EXIT_GAME;
			state->done = 1;
			getMouse()->leftButtonReleased = 0;
		}
	}
	else
		state->mouseOnExit = 0;
}

void drawMainMenuState(MainMenuState* state, Date* date)
{
	drawBitmap(state->background, 0, 0, ALIGN_LEFT);

	drawDate(date);

	if (state->mouseOnPlay)
		drawBitmap(state->playHover, state->playButton->x, state->playButton->y, ALIGN_LEFT);
	else if (state->mouseOnExit)
		drawBitmap(state->exitHover, state->exitButton->x, state->exitButton->y, ALIGN_LEFT);
}

void deleteMainMenuState(MainMenuState* state)
{
	free(state->playButton);
	free(state->exitButton);

	free(state);
}
