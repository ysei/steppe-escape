CC = gcc
SDLDIR = /home/hafron/dev/steppe-escape/src/sdl2
LIBDIR = ${SDLDIR}/lib
INCDIR = ${SDLDIR}/include/SDL2

CFLAGS = -std=c99 -pedantic -Wall -ggdb3 -I${INCDIR} -D_REENTRANT
LDFLAGS = -L${LIBDIR} -Wl,-rpath -Wl,${LIBDIR} -lSDL2 -lSDL_ttf -lSDL_mixer

SRC = se.c utils.c level.c
OBJ = ${SRC:.c=.o}

all: options se

options:
	@echo se build options:
	@echo "CFLAGS   = ${CFLAGS}"
	@echo "LDFLAGS  = ${LDFLAGS}"
	@echo "CC       = ${CC}"

.c.o:
	@echo CC $<
	@${CC} -c ${CFLAGS} $<

se: ${OBJ}
	@echo CC -o $@
	@${CC} -o $@ ${OBJ} ${LDFLAGS}

clean:
	@echo cleaning
	@rm -f se ${OBJ} 

.PHONY: all options clean 
