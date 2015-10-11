#ifndef _GRAPHICS_H
#define _GRAPHICS_H

#include "Mouse.h"

/** @defgroup graphics graphics
 * @{
 *
 * Functions for outputing data to screen in graphics mode
 */

#define SET_TEXT_MODE			0x0003
#define SET_VIDEO_MODE			0x4F02
#define LINEAR_MODEL_BIT 		14

/**
 * @brief Initializes the video module in graphics mode
 *
 * Uses the VBE INT 0x10 interface to set the desired
 *  graphics mode, maps VRAM to the process' address space and
 *  initializes static global variables with the resolution of the screen,
 *  and the number of colors
 *
 * @param mode 16-bit VBE mode to set
 * @return 0 upon success, non-zero upon failure
 */
int initializeGraphics(unsigned short mode);

/**
 * @brief Returns to default Minix 3 text mode (0x03: 25 x 80, 16 colors)
 *
 * @return 0 upon success, non-zero upon failure
 */
int exitGraphics();

/**
 * @brief Fills the display with the given color
 *
 * @param color The display will be filled with this color
 * @return 0 upon success, non-zero upon failure
 */
int fillDisplay(unsigned long color);

/**
 * @brief Copies the buffer to the video memory
 *
 * @return 0 upon success, non-zero upon failure
 */
int swapBuffer();

/**
 * @brief Gets the video memory
 *
 * @return Returns the video memory
 */
char* getGraphicsBuffer();

/**@}*/

#endif /* _GRAPHICS_H */
