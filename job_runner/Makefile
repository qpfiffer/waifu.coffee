CPPFLAGS=-Wall -Werror -g3 -O2
INCLUDES=-I./include

ifndef CC
	CC = clang
endif

all: jobrunner

%.o: ./src/%.cpp
	$(CC) $(CPPFLAGS) $(INCLUDES) -c $< $(CPPLIBS)

jobrunner: main.o
	$(CC) $(CPPFLAGS) $(INCLUDES) -o jobrunner $^ $(CPPLIBS)

