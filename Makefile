PROG = x
SRC = main.c
OBJ = ${SRC:.c=.o}

CC = gcc
INCS = -I/usr/inclde/X11
LIBS = -lm -lX11

LDFLAGS = ${LIBS}
CFLAGS = -Wall -Wextra -O0 ${INCS}

all: ${PROG}

${PROG}: ${OBJ}
	${CC} -o $@ ${LDFLAGS} ${OBJ}

%.o: %.c
	${CC} -c $< ${CFLAGS}

clean:
	-rm ${OBJ} ${PROG}

.PHONY: all clean
