CC	=	gcc
CFLAGS	=	-Wall
all:	spchk

spchk:	spchk.o
	$(CC)	$(CFLAGS)	-o	spchk	spchk.o

spchk.o:	spchk.c
	$(CC)	$(CFLAGS)	-c	spchk.c

clean:
	rm	-f	spchk	spchk.o