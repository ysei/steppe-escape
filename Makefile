CC = g++
SDLDIR = /home/hafron/dev/steppe-ride/src/sdl2
LIBDIR = ${SDLDIR}/lib
INCDIR = ${SDLDIR}/include/SDL2

CFLAGS = -std=c++0x -pedantic -Wall -ggdb3 -I${INCDIR}
LDFLAGS = -L${LIBDIR} -Wl,-rpath -Wl,${LIBDIR} -lSDL2

SRC = sr.cpp
OBJ = ${SRC:.cpp=.o}

all: options sr

options:
	@echo sr build options:
	@echo "CFLAGS   = ${CFLAGS}"
	@echo "LDFLAGS  = ${LDFLAGS}"
	@echo "CC       = ${CC}"

.cpp.o:
	@echo CC $<
	@${CC} -c ${CFLAGS} $<

sr: ${OBJ}
	@echo CC -o $@
	@${CC} -o $@ ${OBJ} ${LDFLAGS}

clean:
	@echo cleaning
	@rm -f sr ${OBJ} 

.PHONY: all options clean 
