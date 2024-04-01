CC      = gcc
CFLAGS  = -g
RM      = rm -f

default: all

all: lkp-enc-app

lkp-enc-app: lkp-enc-app.c
	$(CC) $(CFLAGS) -o lkp-enc-app lkp-enc-app.c


.PHONY: clean

clean:
	$(RM) lkp-enc-app
