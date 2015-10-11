#ifndef _GAMESTATE_H
#define _GAMESTATE_H

#include "Chopper.h"
#include "Bomb.h"
#include "Building.h"
#include "Utilities.h"
#include "Explosion.h"

/** @defgroup mainmenustate mainmenustate
 * @{
 *
 * Functions to manipulate the game
 */

/* Game state actions */
typedef enum
{
	END_GAME, GAME_OVER, GAME_WON, PLAYING
} GameAction;

/*! Represents the game state */
typedef struct
{
	int done; /*!< if 1, the state is over */

	Bitmap* ground; /*!< pointer to the bmp of the ground */
	Bitmap* header; /*!< pointer to the bmp of the header */
	Bitmap* pausedGame; /*!< pointer to the bmp of the paused game */

	Chopper* chopper; /*!< pointer to the struct of the chopper */
	Bomb* bomb; /*!< pointer to the struct of the bomb */
	Explosion* explosions[NUM_EXPLOSIONS]; /*!< pointer to an array of explosions */

	int dropBomb; /*!< if 1, a bomb has been dropped is on the screen */

	Building* buildings[NUM_BUILDINGS]; /*!< pointer to an array of buildings */

	int pause; /*!< if 1, pause game */
	int score; /*!< number of buildings destroyed */

	GameAction action; /*!< action of the game */
} GameState;

/**
 * @brief Creates the game state
 *
 * @return Returns a pointer to the game state
 */
GameState* newGameState();

/**
 * @brief Updates the game state
 *
 * If ESC has been pressed, the chopper has collided width a building (game over) or there are no more buildings (game won) the state is over.
 * Drops bomb if mouse's left button has been released
 *
 * @param state State to update
 * @param scancode Scancode of the key pressed
 * @param timerCounter Timer counter
 */
void updateGameState(GameState* state, unsigned long scancode, int timerCounter);

/**
 * @brief Draws the game state
 *
 * @param state State to draw
 * @param timerCounter Timer counter
 */
void drawGameState(GameState* state, int timerCounter);

/**
 * @brief Destroys the given game state, freeing all resources used by it
 *
 * @param state State to be destroyed
 */
void deleteGameState(GameState* state);

/**
 * @brief Creates a new explosion at the given position
 *
 * Creates bomb at the first NULL position of the explosions array
 *
 * @param state Game state
 * @param x x coordinate of the explosion to create
 * @param y y coordinate of the explosion to create
 */
void createNewExplosion(GameState* state, int x, int y);

/**
 * @brief Detects if the bomb has collided with a building
 *
 * If there is a collision, deletes the block the bomb as collided with and creates a explosion in same position
 *
 * @param state Game state
 */
void detectBombCollisions(GameState* state);

/**
 * @brief Checks if the chopper has collided with a building
 *
 * @return Returns 1 if the chopper has collided with a building, 0 otherwise
 */
int gameOver(GameState* state);

/**
 * @brief Checks if the all buildings are destroyed
 *
 * @return Returns 1 if all buildings are destroyed, 0 otherwise
 */
int gameWon(GameState* state);

/**
 * @brief Shifts explosions array to the left one time
 *
 * @param state Game state
 * @param start Index where to start shifting
 */
void shiftExplosions(GameState* state, int start);

/**
 * @brief Updates the explosions array
 *
 * @param state Game state
 */
void updateExplosions(GameState* state);

/**
 * @brief Draws the buildings
 *
 * @param state Game state
 */
void drawBuildings(GameState* state);

/**
 * @brief Draws the explosions
 *
 * @param state Game state
 */
void drawExplosions(GameState* state);

/**
 * @brief Deletes the buildings
 *
 * @param state Game state
 */
void deleteBuildings(GameState* state);

/**
 * @brief Deletes the explosions
 *
 * @param state Game state
 */
void deleteExplosions(GameState* state);

/**@}*/

#endif /* _GAMESTATE_H */
