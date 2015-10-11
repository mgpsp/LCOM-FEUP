#ifndef _TIMER_H
#define _TIMER_H

/** @defgroup timer timer
 * @{
 *
 * Functions to manipulate the timer
 */

/*! Represents the timer */
typedef struct
{
	int ticked; /*!< if 1, there was a timer interruption */
	unsigned long counter; /*!< number of timer interruptions */
} Timer;

/**
 * @brief Creates a new instance of the timer
 *
 * Creates a timer with the ticked flag and the counter set to 0
 *
 * @return Returns a pointer to the timer
 */
Timer* newTimer();

/**
 * @brief Destroys the given timer, freeing all resources used by it
 *
 * @param timer Timer to delete
 */
void deleteTimer(Timer* timer);

/**
 * @brief Sets the timer ticked flag to 0
 *
 * @param timer Timer to update
 */
void resetTickedFlag(Timer* timer);

/**
 * @brief Increments the counter by one and sets the ticked flag to 1
 *
 * @param timer Timer to update
 */
void timerHandler(Timer* timer);

/**
 * @brief Subscribes and enables Timer 0 interrupts
 *
 * @return Returns bit order in interrupt mask; negative value on failure
 */
int subscribeTimer();

/**
 * @brief Unsubscribes Timer 0 interrupts
 *
 * @return Return 0 upon success and non-zero otherwise
 */
int unsubscribeTimer();

/**@}*/

#endif /* _TIMER_H */
