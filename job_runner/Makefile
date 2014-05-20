CPPFLAGS=-std=c++11 -Wall -Werror -g3 -O2
INCLUDES=-I./include
LIBS=-lzmq -lmsgpack -lkyotocabinet -lm -lstdc++
NAME=jobrunner
CC=clang

#ifndef CC
#	CC = g++
#endif

all: $(NAME)

clean:
	rm *.o
	rm $(NAME)

%.o: ./src/%.cpp
	$(CC) $(CPPFLAGS) $(INCLUDES) -fpic -c $<

jobrunner: main.o waifu.o scheduler.o
	$(CC) $(CPPFLAGS) $(INCLUDES) -o $(NAME) $^ $(LIBS)

