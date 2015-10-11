#ifndef _MAINMENUSTATE_H
#define _MAINMENUSTATE_H

#include "Bitmap.h"
#include "RTC.h"

/** @defgroup mainmenustate mainmenustate
 * @{
 *
 * Functions manipulate the main menu
 */

/*! Represents a rectangle */
typedef struct
{
	int x; /*!< x coordinate of the rectangle */
	int y; /*!< y coordinate of the rectangle */
	int width; /*!< width coordinate of the rectangle */
	int height; /*!< height coordinate of the rectangle */
} Rectangle;

/*! Main menu actions */
typedef enum
{
	PLAY_GAME, EXIT_GAME
} MainMenuAction;

typedef struct
{
	int done; /*!< if 1, the state is over */

	Bitmap* background; /*!< pointer to the bmp of the background */
	Bitmap* playHover; /*!< pointer to the bmp of play button hover */
	Bitmap* exitHover; /*!< pointer to the bmp of exit button hover */

	int mouseOnPlay; /*!< if 1, the mouse in on the play button */
	Rectangle* playButton; /*!< rectangle representing the boundaries of the play button */
	int mouseOnExit; /*!< if 1, the mouse in on the exit button */
	Rectangle* exitButton; /*!< rectangle representing the boundaries of the exit button */

	MainMenuAction action; /*!< action of the main menu */
} MainMenuState;

/**
 * @brief Creates a new rectangle at the given position with the given width and height
 *
 * @param x x coordinate of the rectangle
 * @param y y coordinate of the rectangle
 * @param width width coordinate of the rectangle
 * @param height height coordinate of the rectangle
 * @return Returns a pointer to the rectangle created
 */
Rectangle* newRectangle(int x, int y, int width, int height);;

/**
 * @brief Creates the main menu state
 *
 * @return Returns a pointer to the main menu state
 */
MainMenuState* newMainMenuState();

/**
 * @brief Updates the main menu state
 *
 * If ESC has been pressed or there was a left click inside on the play or exit buttons, the state is over.
 *
 * @param state State to update
 * @param scancode Scancode of the key pressed
 */
void updateMainMenuState(MainMenuState* state, unsigned long scancode);

/**
 * @brief Draws the main menu state
 *
 * @param state State to draw
 */
void drawMainMenuState(MainMenuState* state, Date* date);

/**
 * @brief Destroys the given main menu state, freeing all resources used by it
 *
 * @param state State to be destroyed
 */
void deleteMainMenuState(MainMenuState* state);

/**@}*/

#endif /* _MAINMENUSTATE_H */
