#ifndef _KBC_H
#define _KBC_H

/** @defgroup kbc kbc
 * @{
 *
 * Functions related to the KBC
 */

#define STAT_REG		0x64
#define OUT_BUF 		0x60

#define OBF 			BIT(0)
#define IBF 			BIT(1)
#define AUX 			BIT(5)
#define TO_ERR 			BIT(6)
#define PAR_ERR 		BIT(7)

#define DELAY_US 		20000

/**
 * @brief Reads data from the KBC (Assembly implementation)
 *
 * @return Returns the data read from the KBC
 */
unsigned long readFromKBC_asm();

/**
 * @brief Reads data from the KBC
 *
 * @param ass If 0, call the Assembly implementation
 * @return Returns the data read from the KBC
 */
unsigned long readFromKBC(int ass);

/**
 * @brief Writes data to the KBC
 *
 * @param reg Register where the data will be written
 * @param cmd Command to be written to the KBC
 * @return 0 upon success, non-zero upon failure
 */
int writeToKBC(unsigned long reg, unsigned long cmd);

/**
 * @brief Cleans the KBC buffer
 *
 * @return 0 upon success, non-zero upon failure
 */
int cleanBuffer();

/**@}*/

#endif /* _KBC_H */
