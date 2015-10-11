#ifndef _KEYBOARD_H
#define _KEYBOARD_H

/** @defgroup keyboard keyboard
 * @{
 *
 * Functions to manipulate the keyboard
 */

#define KB_IRQ		1

#define KEY_DOWN(k) (k)  /*!< Checks if a certain key was pressed */
#define KEY_UP(k) (0x80 | k) /*!< Checks if a certain key was released */

/*! Keys */
typedef enum
{
	KEY_ESC = 0x0001,
	KEY_SPACE = 0x0039,
	KEY_P = 0x0019,
	KEY_ENTER = 0x001C
} KEY;

/**
 * @brief Subscribes and enables Keyboard interrupts
 *
 * @return Returns bit order in interrupt mask; negative value on failure
 */
int subscribeKeyboard();

/**
 * @brief Unsubscribes Keyboard interrupts
 *
 * @return Return 0 upon success and non-zero otherwise
 */
int unsubscribeKeyboard();

/**@}*/

#endif /* __KEYBOARD_H */
