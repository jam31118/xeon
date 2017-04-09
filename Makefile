CXX = g++ 
CFLAGS = -O2 -Wall --std=c++11 -I./include
OBJ = obj/main.cpp obj/func.cpp obj/print_func.cpp obj/xeon.cpp

all: main

main: $(OBJ)
	$(CXX) -o $@ $^ $(CFLAGS)
