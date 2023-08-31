CXXFLAGS = -std=c++2a -Wall -pedantic -I../../src/
CXX = g++-10
LINKER = g++-10

all: pddl_and_strips_joint_random_walk

OBJ = pddl_and_strips_joint_random_walk.o

PARSER_SRC := $(wildcard ../../src/ext/mdpsim-parser/*.cc)
PARSER_OBJ := $(PARSER_SRC:%.cc=%.o)

pddl_and_strips_joint_random_walk: $(OBJ) $(PARSER_OBJ)
	+$(MAKE) -C ../../src/ext/mdpsim-parser/
	$(LINKER) $(OBJ) $(PARSER_OBJ) -o $@ $(LDFLAGS)

pddl_and_strips_joint_random_walk.o: pddl_and_strips_joint_random_walk.cc
	$(CXX) $(CXXFLAGS) -c $< -o $@


.PHONY: clean
clean:
	rm -rf $(OBJ) $(PARSER_OBJ) pddl_and_strips_joint_random_walk

