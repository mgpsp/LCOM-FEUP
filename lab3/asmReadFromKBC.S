#include "i8042.h"

.text
.global _asm_readFromKBC

_asm_readFromKBC:
			inb $STATUS_PORT, %al
			testb $OUT_BUF_FULL, %al
			loopz _asm_readFromKBC
			inb $KBD_OUT_BUF, %al
			ret
