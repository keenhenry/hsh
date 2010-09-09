#=======================================
# this is a Makefile for Hank Shell: Hsh 
# @author: Hsiu-Yu Huang 
# @Date  : 02/06/2010
#=======================================

CC = gcc
CFLAGS  = -g -Wall -I.
LDFLAGS = -lreadline

HEAD = list.h hsh.h builtins.h
SRCS = hsh.c list.c builtins.c
OBJS = hsh.o list.o builtins.o
TAR  = hsh

build: all

all: $(TAR) 

$(TAR): $(OBJS)
	$(CC) -g $(OBJS) $(LDFLAGS) -o $(TAR)

$(TAR).o: $(HEAD) builtins.c list.c 


test: build
	valgrind -v --log-file=valgrind.log --tool=memcheck --leak-check=full ./hsh

.PHONY: clean
clean:
	rm -f *.o *.core *~ *.log $(TAR) 
