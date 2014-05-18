CPPFLAGS=-Wall -Werror -g3 -O2
INCLUDES=-I./include
LIBS=-lzmq

ifndef CC
	CC = clang
endif

all: jobrunner

%.o: ./src/%.cpp
	$(CC) $(CPPFLAGS) $(INCLUDES) -c $< $(CPPLIBS)

jobrunner: main.o waifu.o
	$(CC) $(CPPFLAGS) $(INCLUDES) -o jobrunner $^ $(CPPLIBS)

