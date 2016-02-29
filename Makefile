CC = g++

CFLAGS = -std=c++11 -O3

all: cspsolver.cpp
	$(CC) $(CFLAGS) cspsolver.cpp $(OBJS) -o cspsolver $(LIBS)

