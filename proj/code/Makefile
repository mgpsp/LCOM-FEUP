COMPILER_TYPE = gnu

CC = gcc

PROG = proj
SRCS = main.c ChopperDrop.c Chopper.c Bomb.c Building.c Block.c Explosion.c
SRCS += Bitmap.c Graphics.c VBE.c Keyboard.c Mouse.c KBC.c Timer.c RTC.c KBC_asm.S
SRCS += MainMenuState.c GameState.c GameOverState.c GameWonState.c Utilities.c

CCFLAGS = -Wall -03

DPADD += ${LIBDRIVER} ${LIBSYS}
LDADD += -llm -ldriver -lsys

LDFLAGS += -L .

MAN =

.include <bsd.prog.mk>
.include <bsd.gcc.mk>