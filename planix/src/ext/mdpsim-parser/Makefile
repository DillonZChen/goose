# OBJDIR = .obj
DEPDIR = .deps
DEPFLAGS = -MT $@ -MMD -MP -MF $(DEPDIR)/$*.d

SRC = $(wildcard *.cc)
# OBJ = $(SRC:%.cc=$(OBJDIR)/%.o)
OBJ = $(SRC:%.cc=%.o)
DEPFILES = $(SRC:%.cc=$(DEPDIR)/%.d)

CXXFLAGS = -std=c++2a -Wall -pedantic -I../../../src/
# LDFLAGS = -lboost_program_options

OPTFLAGS = -O3 -march=native
DBGFLAGS = -g

YACC	=	/usr/bin/bison -d
LEX	=	/usr/bin/flex
CXX = g++-10 $(CXXFLAGS)
LINKER = g++-10

all: parser.tab.cc tokenizer.cc $(OBJ)

parser.tab.cc: parser.yy
		$(YACC) parser.yy

tokenizer.cc:	tokenizer.ll
		$(LEX) -o tokenizer.cc tokenizer.ll


# $(OBJDIR)/%.o: %.cc $(DEPDIR)/%.d | $(DEPDIR) $(OBJDIR) 
%.o: %.cc $(DEPDIR)/%.d | $(DEPDIR)
	$(CXX) $(DEPFLAGS) -c $< -o $@

$(DEPDIR):
	mkdir -p $@

# $(OBJDIR):
#   mkdir -p $@

$(DEPFILES):

include $(DEPFILES)


.PHONY: clean
clean:
	rm -rf $(DEPDIR)
