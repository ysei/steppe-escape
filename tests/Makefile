CC = gcc

CFLAGS = -std=c99 -pedantic -Wall -ggdb3 

SRC = test_vector.c ../utils.c
OBJ = ${SRC:.c=.o}

all: options run_tests

options:
	@echo se build options:
	@echo "CFLAGS   = ${CFLAGS}"
	@echo "LDFLAGS  = ${LDFLAGS}"
	@echo "CC       = ${CC}"

.c.o:
	@echo CC $<
	@${CC} -c ${CFLAGS} $<

run_tests: ${OBJ}
	@echo CC -o $@
	@${CC} -o $@ ${OBJ} ${LDFLAGS}

clean:
	@echo cleaning
	@rm -f se ${OBJ} 

.PHONY: all options clean 
