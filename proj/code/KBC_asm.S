#define STATUS_PORT		0x64
#define OUT_BUF_FULL	0x01
#define KBD_OUT_BUF		0x60

.text
.global _readFromKBC_asm

_readFromKBC_asm:
			inb $STATUS_PORT, %al
			testb $OUT_BUF_FULL, %al
			loopz _readFromKBC_asm
			inb $KBD_OUT_BUF, %al
			ret
