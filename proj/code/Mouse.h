#ifndef _MOUSE_H
#define _MOUSE_H

#define MOUSE_IRQ					12
#define STAT_REG					0x64
#define OUT_BUF 					0x60
#define WRITE_BYTE_TO_MOUSE 		0xD4
#define SET_STREAM_MODE				0xEA
#define DISABLE_STREAM_MODE			0xF5
#define ENABLE_SENDING_DPACKETS		0xF4
#define ACK							0xFA

/** @defgroup mouse mouse
 * @{
 *
 * Functions manipulate the mouse
 */

/*! Represents the mouse */
typedef struct
{
	int x; /*!< x coordinate of the mouse */
	int y; /*!< x coordinate of the mouse */

	int packetRead; /*!< if 1, a packet has been completely read */
	unsigned long packet[3]; /*!< array with the 3 bytes of the packet */
	int counter; /*!< index of the packet byte being read */

	int leftButtonDown; /*!< if 1, the mouses's left button is pressed */
	int rightButtonDown;/*!< if 1, the mouses's right button is pressed */

	int leftButtonReleased; /*!< if 1, the mouses's left button was just released */
	int rightButtonReleased; /*!< if 1, the mouses's right button was just released */

	int draw; /*!< if 1, draw the mouse cursor*/
} Mouse;

/**
 * @brief Gets the mouse
 *
 * Enables the mouse a creates a new instance if the mouse hasn't been created
 *
 * @return Returns the pointer to the mouse struct
 */
Mouse* getMouse();

/**
 * @brief Updates the mouse
 *
 * Reads from the KBC to the packet and updates mouse flags
 */
void updateMouse();

/**
 * @brief Destroys the mouse, freeing all resources used by it
 */
void deleteMouse();

/**
 * @brief Checks if the mouse is inside a given area
 *
 * @param x1 Top left limit of the area
 * @param x2 Top right limit of the area
 * @param y1 Bottom left limit of the area
 * @param y2 Bottom right limit of the area
 *
 * @return Return 1 if the mouse is inside the given area, 0 otherwise
 */
int mouseInside(int x1, int x2, int y1, int y2);

/**
 * @brief Writes the given command to the mouse
 *
 * Writes the WRITE_BYTE_TO_MOUSE to the KBC and then the given command
 *
 * @return Return 0 upon success and non-zero otherwise
 */
int writeToMouse(unsigned long command);

/**
 * @brief Enables mouse data packets and stream mode
 *
 * @return Return 0 upon success and non-zero otherwise
 */
int enableMouse();

/**
 * @brief Disables stream mode
 *
 * @return Return 0 upon success and non-zero otherwise
 */
int disableMouse();

/**
 * @brief Subscribes and enables Mouse interrupts
 *
 * @return Returns bit order in interrupt mask; negative value on failure
 */
int subscribeMouse();

/**
 * @brief Unsubscribes Mouse interrupts
 *
 * @return Return 0 upon success and non-zero otherwise
 */
int unsubscribeMouse();

/**@}*/

#endif /* _MOUSE_H */
