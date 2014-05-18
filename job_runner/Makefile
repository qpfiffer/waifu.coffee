CPPFLAGS=-Wall -Werror -g3 -O2
INCLUDES=-I./include
LIBS=-lzmq -lstdc++
NAME=jobrunner

ifndef CC
	CC = clang
endif

all: $(NAME)

clean:
	rm *.o
	rm $(NAME)

%.o: ./src/%.cpp
	$(CC) $(CPPFLAGS) $(INCLUDES) -c $< $(LIBS)

jobrunner: main.o waifu.o
	$(CC) $(CPPFLAGS) $(INCLUDES) -o $(NAME) $^ $(LIBS)

