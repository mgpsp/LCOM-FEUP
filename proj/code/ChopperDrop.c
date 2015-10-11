#include <minix/drivers.h>

#include "ChopperDrop.h"
#include "Keyboard.h"
#include "Graphics.h"
#include "Utilities.h"
#include "Mouse.h"
#include "KBC.h"
#include "MainMenuState.h"
#include "GameState.h"
#include "GameOverState.h"
#include "GameWonState.h"

void checkIfStateIsDone(ChopperDrop* game);
void deleteCurrentState(ChopperDrop* game);

ChopperDrop* initializeChopperDrop()
{
	ChopperDrop* game = (ChopperDrop*) malloc(sizeof(ChopperDrop));

	// Subscribe devices
	game->IRQ_SET_KB = subscribeKeyboard();
	game->IRQ_SET_TIMER = subscribeTimer();
	game->IRQ_SET_MOUSE = subscribeMouse();
	game->IRQ_SET_RTC = subscribeRTC();

	game->done = 0;
	game->draw = 1;
	game->scancode = 0;
	game->timer = newTimer();
	game->date = newDate();

	game->currentState = MAIN_MENU_STATE;
	game->state = newMainMenuState();

    game->mouseCursor = loadBitmap("/home/images/cursor.bmp");

    // Load digits
    int i;
    for (i = 0; i < 10; i++)
    {
    	char filename[40];
    	sprintf(filename, "/home/images/digits/%d.bmp", i);
    	digits[i] = loadBitmap(filename);
    }

    slash = loadBitmap("/home/images/slash.bmp");
    colon = loadBitmap("/home/images/colon.bmp");

	return game;
}

void updateChopperDrop(ChopperDrop* game)
{
	int ipc_status;
	message msg;

	resetTickedFlag(game->timer);

	if (driver_receive(ANY, &msg, &ipc_status) != 0)
		return;

	if (is_ipc_notify(ipc_status))
	{
		switch (_ENDPOINT_P(msg.m_source))
		{
		case HARDWARE:
			// Keyboard
			if (msg.NOTIFY_ARG & game->IRQ_SET_KB)
				game->scancode = readFromKBC(0);

			// Timer
			if (msg.NOTIFY_ARG & game->IRQ_SET_TIMER)
				timerHandler(game->timer);

			// Mouse
			if (msg.NOTIFY_ARG & game->IRQ_SET_MOUSE)
				updateMouse();

			// RTC
			if (msg.NOTIFY_ARG & game->IRQ_SET_RTC)
				updateDate(game->date);
			break;
		default:
			break;
		}
	}

	if (game->timer->ticked)
	{
		// Update game states and mouse at 60 FPS

		getMouse()->draw = 1;

		switch(game->currentState)
		{
		case MAIN_MENU_STATE:
			updateMainMenuState(game->state, game->scancode);
			break;
		case GAME_STATE:
			updateGameState(game->state, game->scancode, game->timer->counter);
			break;
		case GAME_OVER_STATE:
			updateGameOverState(game->state, game->scancode);
			break;
		case GAME_WON_STATE:
			updateGameWonState(game->state, game->scancode);
			break;
		default:
			break;
		}

		game->scancode = 0;
		game->draw = 1;
	}

	checkIfStateIsDone(game);
}

void drawMouse(ChopperDrop* game)
{
	drawBitmap(game->mouseCursor, getMouse()->x, getMouse()->y, ALIGN_LEFT);
	getMouse()->draw = 0;
}

void drawChopperDrop(ChopperDrop* game)
{
	switch(game->currentState)
	{
	case MAIN_MENU_STATE:
		drawMainMenuState(game->state, game->date);
		break;
	case GAME_STATE:
		drawGameState(game->state, game->timer->counter);
		break;
	case GAME_OVER_STATE:
		drawGameOverState(game->state);
		break;
	case GAME_WON_STATE:
		drawGameWonState(game->state);
		break;
	default:
		break;
	}

	game->draw = 0;
}

void stopChopperDrop(ChopperDrop* game)
{
	// unsubscribe devices
	unsubscribeKeyboard();
	unsubscribeTimer();
	unsubscribeMouse();

	deleteBitmap(game->mouseCursor);

	deleteCurrentState(game);

	disableMouse();
	cleanBuffer();
	deleteMouse();
	deleteTimer(game->timer);
	deleteDate(game->date);

	free(game);
}

void changeState(ChopperDrop* game, State newState)
{
	deleteCurrentState(game);

	game->currentState = newState;

	switch(game->currentState)
	{
	case MAIN_MENU_STATE:
		game->state = newMainMenuState();
		break;
	case GAME_STATE:
		game->state = newGameState();
		break;
	case GAME_OVER_STATE:
		game->state = newGameOverState();
		break;
	case GAME_WON_STATE:
		game->state = newGameWonState();
		break;
	default:
		break;
	}

	game->draw = 1;
}

void checkIfStateIsDone(ChopperDrop* game)
{
	switch (game->currentState)
	{
	case MAIN_MENU_STATE:
		if (((MainMenuState*) (game->state))->done)
		{
			MainMenuAction action = ((MainMenuState*) (game->state))->action;

			switch (action)
			{
			case PLAY_GAME:
				changeState(game, GAME_STATE);
				break;
			case EXIT_GAME:
				game->done = 1;
				break;
			default:
				break;
			}
		}
		break;
	case GAME_STATE:
		if (((GameState*) (game->state))->done)
		{
			GameAction action = ((GameState*) (game->state))->action;

			switch (action)
			{
			case END_GAME:
				changeState(game, MAIN_MENU_STATE);
				break;
			case GAME_OVER:
				changeState(game, GAME_OVER_STATE);
				break;
			case GAME_WON:
				changeState(game, GAME_WON_STATE);
				break;
			default:
				break;
			}
		}
		break;
	case GAME_OVER_STATE:
		if (((GameOverState*) (game->state))->done)
			changeState(game, MAIN_MENU_STATE);
		break;
	case GAME_WON_STATE:
		if (((GameWonState*) (game->state))->done)
			changeState(game, MAIN_MENU_STATE);
		break;
	default:
		break;
	}
}

void deleteCurrentState(ChopperDrop* game)
{
	switch (game->currentState)
	{
	case MAIN_MENU_STATE:
		deleteMainMenuState(game->state);
		break;
	case GAME_STATE:
		deleteGameState(game->state);
		break;
	case GAME_OVER_STATE:
		deleteGameOverState(game->state);
		break;
	case GAME_WON_STATE:
		deleteGameWonState(game->state);
		break;
	default:
		break;
	}
}

