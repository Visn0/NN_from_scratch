CC=g++
OPTIONS= -g -std=c++20 -Wall -pedantic -o3
LIBDIR=lib
INCLUDEDIR=include
_OBJ= net_t.o
OBJ= $(patsubst %,$(LIBDIR)/%,$(_OBJ))
MAIN=src

main: $(MAIN)/main.cpp $(OBJ)
	$(CC) $(OPTIONS) -I$(INCLUDEDIR) $(MAIN)/main.cpp $(OBJ) -o main

test: main.cpp $(OBJ)
	$(CC) $(OPTIONS) -I$(INCLUDEDIR) main.cpp $(OBJ) -o main

$(LIBDIR)/%.o : $(LIBDIR)/%.cpp $(INCLUDEDIR)/%.h
	$(CC) $(OPTIONS) -c -I$(INCLUDEDIR) -o $@ $<

clean:
	rm -rf $(OBJ) $(MAIN)/main.cpp $(MAIN)/*.cpp.out