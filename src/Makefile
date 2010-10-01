#=======================================
# this is a Makefile for Hank Shell: Hsh 
# @author: Hsiu-Yu Huang 
# @Date  : 02/06/2010
#=======================================

CC = gcc
CFLAGS  = -g -Wall -I.
LDFLAGS = -lreadline

HEAD = list.h hsh.h
SRCS = hsh.c list.c builtins.c main.c io_redirect.c pipe.c
OBJS = hsh.o list.o builtins.o main.o io_redirect.o pipe.o
TAR  = hsh

build: all

all: $(TAR) 

$(TAR): $(OBJS)
	$(CC) -g $(OBJS) $(LDFLAGS) -o $(TAR)

$(TAR).o: $(HEAD) main.c builtins.c list.c io_redirect.c pipe.c

test: build
	valgrind -v --log-file=valgrind.log --tool=memcheck --leak-check=full ./hsh

.PHONY: clean
clean:
	rm -f *.o *.core *~ *.log $(TAR) 
