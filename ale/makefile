CC=g++
OPTIONS= -g -std=c++20 -Wall -pedantic -o3 -lale -L. -Wl,-rpath=. -lSDL
LIBDIR=../lib
INCLUDEDIR=../include
_OBJ= net_t.o
OBJ= $(patsubst %,$(LIBDIR)/%,$(_OBJ))

ale: minimal_agent.cpp $(OBJ)
	$(CC) $(OPTIONS) -I$(INCLUDEDIR) minimal_agent.cpp $(OBJ) -o ale

$(LIBDIR)/%.o : $(LIBDIR)/%.cpp $(INCLUDEDIR)/%.h
	$(CC) $(OPTIONS) -c -I$(INCLUDEDIR) -o $@ $<

clean:
	rm ale