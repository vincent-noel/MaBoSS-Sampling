#
# MaBoSS project Makefile to compile parameter sampling
#
# Institut Curie, France
# Vincent Noël

CC = g++
FLAGS = -std=c++11

SRC_DIR = src

MABOSS_SRC_DIR = src/MaBoSS-env-2.0/engine/src
MABOSS_INCLUDE_DIR = src/MaBoSS-env-2.0/engine/include
MABOSS_LIB_DIR = src/MaBoSS-env-2.0/engine/lib

all: clean sampling example

maboss_lib: $(MABOSS_LIB_DIR)/libmaboss.so

$(MABOSS_LIB_DIR)/libmaboss.so:
	make -C $(MABOSS_SRC_DIR) install_lib

$(MABOSS_INCLUDE_DIR)/MaBEstEngine.h:
	make -C $(MABOSS_SRC_DIR) install_lib


main.o : $(SRC_DIR)/main.c $(MABOSS_INCLUDE_DIR)/MaBEstEngine.h
	$(CC) $(FLAGS) -c $(SRC_DIR)/main.c -I$(MABOSS_INCLUDE_DIR) -o main.o

PSetSimulation.o : $(SRC_DIR)/PSetSimulation.h $(SRC_DIR)/PSetSimulation.cc
	$(CC) $(FLAGS) -c $(SRC_DIR)/PSetSimulation.cc -I$(MABOSS_INCLUDE_DIR) -o PSetSimulation.o 

SamplingParameters.o : $(SRC_DIR)/SamplingParameters.h $(SRC_DIR)/SamplingParameters.cc
	$(CC) $(FLAGS) -c $(SRC_DIR)/SamplingParameters.cc -I$(MABOSS_INCLUDE_DIR) -o SamplingParameters.o 

sampling: main.o $(MABOSS_LIB_DIR)/libmaboss.so PSetSimulation.o SamplingParameters.o
	$(CC) $(FLAGS) main.o PSetSimulation.o SamplingParameters.o $(MABOSS_LIB_DIR)/libmaboss.so -o MaBoSS-Sampling
	rm -f *.o

# Example to show why we need to reparse network and config for each simulation
example.o: $(SRC_DIR)/example.c $(MABOSS_INCLUDE_DIR)/MaBEstEngine.h
	$(CC) $(FLAGS) -c $(SRC_DIR)/example.c -I$(MABOSS_INCLUDE_DIR) -o example.o 

example: example.o $(MABOSS_LIB_DIR)/libmaboss.so 
	$(CC) $(FLAGS) example.o $(MABOSS_LIB_DIR)/libmaboss.so -o example
	rm -f *.o

# Test
test:
	make
	./MaBoSS-Sampling -c examples/Four_cycle.cfg -v '0, 1' examples/Four_cycle.bnd > result
	diff tests/expected_result result

clean: 
	rm -f MaBoSS-Sampling example $(MABOSS_LIB_DIR)/libmaboss.so $(MABOSS_INCLUDE_DIR)/include/*.h
