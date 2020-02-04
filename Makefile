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

all: clean MaBoSS-Sampling example MaBoSS-Sampling_128n

maboss_alib: $(MABOSS_LIB_DIR)/libMaBoSS.a

$(MABOSS_LIB_DIR)/libMaBoSS.a: $(MABOSS_SRC_DIR)/*.cc $(MABOSS_SRC_DIR)/*.h
	make -C $(MABOSS_SRC_DIR) install_alib

$(MABOSS_LIB_DIR)/libMaBoSS_128n.a: $(MABOSS_SRC_DIR)/*.cc $(MABOSS_SRC_DIR)/*.h
	make -C $(MABOSS_SRC_DIR) MAXNODES=128 install_alib

$(MABOSS_INCLUDE_DIR)/MaBEstEngine.h:
	make -C $(MABOSS_SRC_DIR) install_alib


main.o : $(SRC_DIR)/main.c $(MABOSS_INCLUDE_DIR)/MaBEstEngine.h
	$(CC) $(FLAGS) -c $(SRC_DIR)/main.c -I$(MABOSS_INCLUDE_DIR) -o main.o

PSetSimulation.o : $(SRC_DIR)/PSetSimulation.h $(SRC_DIR)/PSetSimulation.cc
	$(CC) $(FLAGS) -c $(SRC_DIR)/PSetSimulation.cc -I$(MABOSS_INCLUDE_DIR) -o PSetSimulation.o 

SamplingParameters.o : $(SRC_DIR)/SamplingParameters.h $(SRC_DIR)/SamplingParameters.cc
	$(CC) $(FLAGS) -c $(SRC_DIR)/SamplingParameters.cc -I$(MABOSS_INCLUDE_DIR) -o SamplingParameters.o 

MaBoSS-Sampling: main.o $(MABOSS_LIB_DIR)/libMaBoSS.a PSetSimulation.o SamplingParameters.o
	$(CC) $(FLAGS) main.o PSetSimulation.o SamplingParameters.o $(MABOSS_LIB_DIR)/libMaBoSS.a -ljsoncpp -lpthread -ldl -o MaBoSS-Sampling

main_128n.o : $(SRC_DIR)/main.c $(MABOSS_INCLUDE_DIR)/MaBEstEngine.h
	$(CC) $(FLAGS) -c $(SRC_DIR)/main.c -I$(MABOSS_INCLUDE_DIR) -DMAXNODES=128 -o main_128n.o

PSetSimulation_128n.o : $(SRC_DIR)/PSetSimulation.h $(SRC_DIR)/PSetSimulation.cc
	$(CC) $(FLAGS) -c $(SRC_DIR)/PSetSimulation.cc -I$(MABOSS_INCLUDE_DIR) -DMAXNODES=128 -o PSetSimulation_128n.o 

SamplingParameters_128n.o : $(SRC_DIR)/SamplingParameters.h $(SRC_DIR)/SamplingParameters.cc
	$(CC) $(FLAGS) -c $(SRC_DIR)/SamplingParameters.cc -I$(MABOSS_INCLUDE_DIR) -DMAXNODES=128 -o SamplingParameters_128n.o 

MaBoSS-Sampling_128n: main_128n.o $(MABOSS_LIB_DIR)/libMaBoSS_128n.a PSetSimulation_128n.o SamplingParameters_128n.o
	$(CC) $(FLAGS) main_128n.o PSetSimulation_128n.o SamplingParameters_128n.o $(MABOSS_LIB_DIR)/libMaBoSS_128n.a -ljsoncpp -lpthread -ldl -o MaBoSS-Sampling_128n

# Example to show why we need to reparse network and config for each simulation
example.o: $(SRC_DIR)/example.c $(MABOSS_INCLUDE_DIR)/MaBEstEngine.h
	$(CC) $(FLAGS) -c $(SRC_DIR)/example.c -I$(MABOSS_INCLUDE_DIR) -lpthread -ldl -o example.o 

example: example.o $(MABOSS_LIB_DIR)/libMaBoSS.a
	$(CC) $(FLAGS) example.o $(MABOSS_LIB_DIR)/libMaBoSS.a -lpthread -ldl -o example

# Test
test:
	make MaBoSS-Sampling
	./MaBoSS-Sampling -c examples/Four_cycle.cfg -v '0, 1' examples/Four_cycle.bnd > result
	diff tests/expected_result result
	./MaBoSS-Sampling -c examples/Four_cycle.cfg -s examples/settings.json examples/Four_cycle.bnd > result
	diff tests/expected_result_wsettings result

test_128n:
	make MaBoSS-Sampling_128n
	./MaBoSS-Sampling_128n -c examples/Four_cycle.cfg -v '0, 1' examples/Four_cycle.bnd > result
	diff tests/expected_result result
	./MaBoSS-Sampling_128n -c examples/Four_cycle.cfg -s examples/settings.json examples/Four_cycle.bnd > result
	diff tests/expected_result_wsettings result


clean: 
	rm -f MaBoSS-Sampling example $(MABOSS_LIB_DIR)/libMaBoSS.so $(MABOSS_LIB_DIR)/libMaBoSS.a $(MABOSS_INCLUDE_DIR)/include/*.h *.o
