#ifndef _GAMEOVERSTATE_H
#define _GAMEOVERSTATE_H

#include "Bitmap.h"

/*! Represents the game over state */
typedef struct
{
	int done; /*!< if 1, the state is over */

	Bitmap* background; /*!< pointer to the bmp of the background */
} GameOverState;

/**
 * @brief Creates the game over state
 *
 * @return Returns a pointer to the game over state
 */
GameOverState* newGameOverState();

/**
 * @brief Updates the game over state
 *
 * If enter has been pressed, the state is over.
 *
 * @param state State to update
 * @param scancode Scancode of the key pressed
 */
void updateGameOverState(GameOverState* state, unsigned long scancode);

/**
 * @brief Draws the game over state
 *
 * @param state State to draw
 */
void drawGameOverState(GameOverState* state);

/**
 * @brief Destroys the given game over state, freeing all resources used by it
 *
 * @param state State to be destroyed
 */
void deleteGameOverState(GameOverState* state);

/**@}*/

#endif /* _GAMEOVERSTATE_H */
