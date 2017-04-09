CXX = g++ 
CFLAGS = -O2 -Wall --std=c++11 -I./include
OBJ = main.cpp func.cpp print_func.cpp

all: main

main: $(OBJ)
	$(CXX) -o $@ $^ $(CFLAGS)
