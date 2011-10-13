CFLAGS = -Wall

CC = g++

INCLUDES = -Irunlib-for-linux/

all: runexe

w32invoke:
	${CC} ${CFLAGS} -c runlib-for-linux/w32invoke.cpp

runexe: w32invoke
	${CC} ${CFLAGS} ${INCLUDES} -o $@ Configuration.cpp InvocationVerdict.cpp Run.cpp Strings.cpp InvocationResult.cpp InvocationParams.cpp EntryPoint.cpp w32invoke.o 

clean:
	rm -rf *.o
	rm -rf runexe
