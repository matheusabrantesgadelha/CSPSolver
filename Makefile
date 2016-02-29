CC = g++

CFLAGS = -std=c++11 -ggdb

all: cspsolver.cpp
	$(CC) $(CFLAGS) cspsolver.cpp $(OBJS) -o cspsolver $(LIBS)

