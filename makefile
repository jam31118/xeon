CXX = g++ 
CFLAGS = -O2 -Wall --std=c++11 -I./include
OBJ = lib/* 
OUTPUT = xeon
SUBDIRS = $(wildcard */.)

all: main

main: $(OBJ)
	$(CXX) -o $(OUTPUT) $^ $(CFLAGS)

clean: clean-exe clean-outfile

clean-exe: 
	rm -f $(OUTPUT)

clean-outfile: $(SUBDIRS)
	for subdir in $(SUBDIRS); do rm -f $${subdir}/*.o; done

