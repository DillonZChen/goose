CXXFLAGS = -std=c++2a -Wall -pedantic -I../../src/
CXX = g++-10
LINKER = g++-10

all: mo-sas-parser-example

OBJ = mo-sas-parser-example.o sas-translator.o

PARSER_SRC := $(wildcard ../../src/ext/fast-downward-parser/*.cc)
PARSER_OBJ := $(PARSER_SRC:%.cc=%.o)

mo-sas-parser-example: $(OBJ) $(PARSER_SRC)
	+$(MAKE) -C ../../src/ext/fast-downward-parser/
	$(LINKER) $(OBJ) $(PARSER_OBJ) -o $@ $(LDFLAGS)

mo-sas-parser-example.o: mo-sas-parser-example.cc
	$(CXX) $(CXXFLAGS) -c $< -o $@

sas-translator.o: ../../src/representations/translator.h ../../src/representations/translator.cc
	$(CXX) $(CXXFLAGS) -c ../../src/representations/translator.cc -o sas-translator.o


.PHONY: clean
clean:
	rm -rf $(OBJ) mo-sas-parser-example

