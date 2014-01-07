CC = gcc
SDLDIR = /home/hafron/dev/steppe-escape/src/sdl2
LIBDIR = ${SDLDIR}/lib
INCDIR = ${SDLDIR}/include/SDL2

CFLAGS = -std=c99 -pedantic -Wall -ggdb3 -I${INCDIR}
LDFLAGS = -L${LIBDIR} -Wl,-rpath -Wl,${LIBDIR} -lSDL2

COPY_FILES = utils.c level.c level.h utils.h

utils_SRC = test_utils.c utils.c
utils_OBJ = ${utils_SRC:.c=.o}

level_SRC = test_level.c utils.c level.c
level_OBJ = ${level_SRC:.c=.o}

all: clean run_test_utils run_test_level

${COPY_FILES}: 
	(cd ../ && cp ${COPY_FILES} tests)
.c.o:
	@echo CC $<
	@${CC} -c ${CFLAGS} $<

run_test_utils: ${utils_OBJ}
	@echo CC -o $@
	@${CC} -o $@ ${utils_OBJ} ${LDFLAGS}

run_test_level: ${level_OBJ}
	@echo CC -o $@
	@${CC} -o $@ ${level_OBJ} ${LDFLAGS}

clean:
	@echo cleaning
	@rm -f run_test_utils run_test_level ${OBJ} ${COPY_FILES}

.PHONY: all clean 
