#ifndef _CHOPPERDROP_H
#define _CHOPPERDROP_H

#include "Bitmap.h"
#include "Timer.h"
#include "RTC.h"

/** @defgroup chopperdrop chopperdrop
 * @{
 *
 * Functions to switch between states
 */

/*! States */
typedef enum
{
	MAIN_MENU_STATE, GAME_STATE, GAME_OVER_STATE, GAME_WON_STATE
} State;

/*! Represents the game */
typedef struct
{
	int IRQ_SET_KB; /*!< order of the keyboard bit in the interrupt mask */
	int IRQ_SET_TIMER; /*!< order of the timer bit in the interrupt mask */
	int IRQ_SET_MOUSE; /*!< order of the mouse bit in the interrupt mask */
	int IRQ_SET_RTC; /*!< order of the RTC bit in the interrupt mask */

	int done; /*!< if 1, exit game */
	int draw; /*!< if 1, draw game */

	unsigned long scancode; /*!< scancode of the key pressed */

	Timer* timer; /*!< pointer to the timer struct */
	Date* date; /*!< pointer to the date struct */

	Bitmap* mouseCursor; /*!< pointer to the bitmap of the mouse cursor */

	State currentState; /*!< indicates the current state */
	void* state; /*!< void pointer to the current state */
} ChopperDrop;

/**
 * @brief Initializes the game
 *
 * Subscribes devices, creates the timer and date structs and sets to current state the main menu.
 *
 * @return Returns a pointer to the struct created
 */
ChopperDrop* initializeChopperDrop();

/**
 * @brief Updates the given game
 *
 * Checks for device interruptions and updates the current state.
 *
 * @param game Game to update
 */
void updateChopperDrop(ChopperDrop* game);

/**
 * @brief Draws the current state
 *
 * @param game Game to draw
 */
void drawChopperDrop(ChopperDrop* game);

/**
 * @brief Destroys the given game, freeing all resources used by it
 *
 * Unsubscribes devices and deletes mouse, timer and date.
 *
 * @param game Game to be deleted
 */
void stopChopperDrop(ChopperDrop* game);

/**
 * @brief Draws the mouse cursor
 *
 * @param game Pointer to the game struct
 */
void drawMouse(ChopperDrop* game);

/**
 * @brief Changes state
 *
 * @param game Pointer to the game struct
 * @param newState State to change to
 */
void changeState(ChopperDrop* game, State newState);

/**
 * @brief Checks if the current state is over
 *
 * @param game Pointer to the game struct
 */
void checkIfStateIsDone(ChopperDrop* game);

/**
 * @brief Destroys the current state, freeing all resources used by it
 *
 * @param game Pointer to the game struct
 */
void deleteCurrentState(ChopperDrop* game);

/**@}*/

#endif /* _CHOPPERDROP_H */
