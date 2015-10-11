#ifndef _RTC_H
#define _RTC_H

#include "Utilities.h"

/** @defgroup rtc rtc
 * @{
 *
 * Functions manipulate the RTC
 */

#define RTC_ADDR_REG	0x70
#define RTC_DATA_REG	0x71
#define RTC_IRQ		8

#define REG_A		10
#define REG_B  		11
#define REG_C  		12
#define UIP			BIT(0)

#define SECONDS		0
#define MINUTES		2
#define HOURS		4
#define MONTH_DAY	7
#define MONTH		8
#define YEAR		9

/*! Represents the date */
typedef struct
{
	int second; /*!< second of the current date*/
	int minute; /*!< minute of the current date*/
	int hour; /*!< hour of the current date*/
	int month; /*!< month of the current date*/
	int year; /*!< year of the current date*/
	int monthDay; /*!< day of the month of the current date*/
} Date;

/**
 * @brief Creates a new instance of the date
 *
 * @return Returns a pointer to the new struct created
 */
Date* newDate();

/**
 * @brief Updates date
 *
 * Reads the registers containing the date information and updates the given date struct
 *
 * @param date Date struct to update
 */
void updateDate(Date* date);

/**
 * @brief Draws the given date
 *
 * @param date Date to draw
 */
void drawDate(Date* date);

/**
 * @brief Destroys the given date, freeing all resources used by it
 *
 * @param date Date to be destroyed
 */
void deleteDate(Date* date);

/**
 * @brief Converts a number in BCD to binary
 *
 * @return Returns the given number in binary
 */
unsigned char BCDtoBinary(unsigned char bcd);

/**
 * @brief Subscribes and enables RTC interrupts
 *
 * @return Returns bit order in interrupt mask; negative value on failure
 */
int subscribeRTC();

/**
 * @brief Unsubscribes Timer 0 interrupts
 *
 * @return Return 0 upon success and non-zero otherwise
 */
int unsubscribeRTC();

/**@}*/

#endif /* _RTC_H */
