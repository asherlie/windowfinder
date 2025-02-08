CC=gcc
CFLAGS= -Wall -Wextra -Wpedantic -Werror -O3 -lX11

all: wf

wf: wf.c

.PHONY:
clean:
	rm -f wf
