#ifndef __I8042_H
#define __I8042_H

#define BIT(n) 					(0x01<<(n))

#define DELAY_US 				20000

#define MOUSE_IRQ				12
#define TIMER0_IRQ				0

#define STAT_REG				0x64
#define KBC_CMD_REG 			0x64
#define OUT_BUF 				0x60
#define KBC_CMD_ED				(BIT(7) | BIT(6) | BIT(5) | BIT(3) | BIT(2) | BIT(0))
#define ACK						0xFA

#define ACTIVATE_MOUSE			0xA8
#define DISABLE_SMODE			0xF5
#define WRITE_BYTE_TO_MOUSE 	0xD4
#define ENABLE_SENDING_DPACKETS	0xF4
#define SET_SMODE				0xEA
#define STATUS_REQUEST			0xE9

#define OBF 					BIT(0)
#define IBF 					BIT(1)
#define AUX 					BIT(5)
#define TO_ERR 					BIT(6)
#define PAR_ERR 				BIT(7)

#endif
