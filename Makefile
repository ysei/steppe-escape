CC = gcc

CFLAGS = -std=c99 -pedantic -Wall -ggdb3 -I/usr/include/SDL2 -D_GNU_SOURCE=1 -D_REENTRANT
LDFLAGS = -L/usr/lib -lpthread -lSDL2 -lSDL2_ttf -lSDL2_mixer -lSDL2_image

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
