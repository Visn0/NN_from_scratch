CC=g++
OPTIONS= -g -O3 -std=c++2a -lale -L. -Wl,-rpath=. -lSDL
LIBDIR=../lib
INCLUDEDIR=../include
_OBJ= net_t.o
OBJ= $(patsubst %,$(LIBDIR)/%,$(_OBJ))

ale: minimal_agent.cpp $(OBJ)
	$(CC) minimal_agent.cpp $(OBJ) -o C19RETRO.robot $(OPTIONS) -I$(INCLUDEDIR) 

$(LIBDIR)/%.o : $(LIBDIR)/%.cpp $(INCLUDEDIR)/%.h
	$(CC) $(OPTIONS) -c -I$(INCLUDEDIR) -o $@ $<

clean:
	rm C19RETRO.robot $(OBJ)