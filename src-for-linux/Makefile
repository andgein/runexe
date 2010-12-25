CFLAGS = -Wall

CC = g++

INCLUDES = -I.

CFLAGS = -g -rdynamic

all: runexe

runexe: process.cpp process.h EntryPoint.cpp
	${CC} ${CFLAGS} ${INCLUDES} -o $@ w32invoke.cpp process.cpp ../src/Configuration.cpp ../src/InvocationVerdict.cpp ../src/Run.cpp ../src/Strings.cpp ../src/InvocationResult.cpp ../src/InvocationParams.cpp EntryPoint.cpp

clean:
	rm -rf *.o
	rm -rf runexe
