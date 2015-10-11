#ifndef _GAMEWONSTATE_H
#define _GAMEWONSTATE_H

#include "Bitmap.h"

/*! Represents the game won state */
typedef struct
{
	int done; /*!< if 1, the state is over */

	Bitmap* background; /*!< pointer to the bmp of the background*/
} GameWonState;

/**
 * @brief Creates the game won state
 *
 * @return Returns a pointer to the game won state
 */
GameWonState* newGameWonState();

/**
 * @brief Updates the game won state
 *
 * If enter has been pressed, the state is over.
 *
 * @param state State to update
 * @param scancode Scancode of the key pressed
 */
void updateGameWonState(GameWonState* state, unsigned long scancode);

/**
 * @brief Draws the game won state
 *
 * @param state State to draw
 */
void drawGameWonState(GameWonState* state);

/**
 * @brief Destroys the given game won state, freeing all resources used by it
 *
 * @param state State to be destroyed
 */
void deleteGameWonState(GameWonState* state);

/**@}*/

#endif /* _GAMEWONSTATE_H */
