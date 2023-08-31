OBJ = axiom.o helper_functions.o mutex_group.o operator.o state.o variable.o problem.o

CFLAGS += --std=c++2a --pedantic -Wall
C++	=	g++-10

all: $(OBJ)

clean:
	rm -f $(OBJ)

.cc.o:
		$(C++) $(CFLAGS) $(INCLUDE) -c $<

