CXX = g++ -O2 -Wall --std=c++11

main: main.cpp func.cpp print_func.cpp
	$(CXX) -o main main.cpp func.cpp print_func.cpp
