#ifndef __KEYBOARD_H
#define __KEYBOARD_H

// Constants for programming the keyboard

#define BIT(n) 			(0x01<<(n))

#define DELAY_US 		20000

#define KB_IRQ			1
#define TIMER0_IRQ		0

#define STAT_REG		0x64
#define KBC_CMD_REG 	0x64
#define OUT_BUF 		0x60
#define KBC_CMD_ED		(BIT(7) | BIT(6) | BIT(5) | BIT(3) | BIT(2) | BIT(0))
#define KB_ACK 			0xFA

#define OBF 			BIT(0)
#define IBF 			BIT(1)
#define AUX 			BIT(5)
#define TO_ERR 			BIT(6)
#define PAR_ERR 		BIT(7)

#define ESC				0x81

#define BREAKCODE 		BIT(7)

// Assembly constants
#define STATUS_PORT		0x64
#define OUT_BUF_FULL	0x01
#define KBD_OUT_BUF 	0x60

#endif
