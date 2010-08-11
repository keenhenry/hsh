#=======================================
# this is a Makefile for Hank Shell: Hsh 
# @author: Hsiu-Yu Huang 
# @Date  : 02/06/2010
#=======================================

CC = gcc
CFLAGS  = -g -Wall -I.
LDFLAGS = -lreadline

HEAD = list.h hsh.h
SRCS = hsh.c list.c 
OBJS = hsh.o list.o
TAR  = hsh

build: all

all: $(TAR) 

$(TAR): $(OBJS)
	$(CC) -g $(OBJS) $(LDFLAGS) -o $(TAR)

$(TAR).o: $(HEAD) list.c 

#list.o: list.h

test: build
	valgrind --leak-check=yes ./hsh

.PHONY: clean
clean:
	rm -f *.o *.core *~ $(TAR) 
