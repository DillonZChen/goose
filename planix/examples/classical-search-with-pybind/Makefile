PYBIND_FLAGS = $(shell python3 -m pybind11 --includes) -fvisibility=hidden

OPTS = -O3
CXXFLAGS = -std=c++2a -Wall -pedantic -I../../src/ $(OPTS) -fPIC $(PYBIND_FLAGS)
CXX = g++-10
LINKER = g++-10
LDFLAGS =

all: main

OBJ = main.o

PARSER_SRC := $(wildcard ../../src/ext/mdpsim-parser/*.cc)
PARSER_OBJ := $(PARSER_SRC:%.cc=%.o)

HEURISTIC_SRC := pybind_goal_count.cc
HEURISTIC_OBJ := $(HEURISTIC_SRC:%.cc=%.o)

main: $(OBJ) $(PARSER_OBJ) $(HEURISTIC_OBJ)
	+$(MAKE) -C ../../src/ext/mdpsim-parser/
	$(LINKER) $(OBJ) $(PARSER_OBJ) $(HEURISTIC_OBJ) -lpython3.10 -o $@ $(LDFLAGS)

main.o: main.cc
	$(CXX) $(CXXFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -rf $(OBJ) $(PARSER_OBJ) main
