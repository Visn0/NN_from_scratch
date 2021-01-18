CC=g++
OPTIONS= -g -std=c++20 -Wall -pedantic -o3
LIBDIR=lib
INCLUDEDIR=include
_OBJ= globals.o net_t.o
OBJ= $(patsubst %,$(LIBDIR)/%,$(_OBJ))
MAIN=src

main: $(MAIN)/main.cpp $(OBJ)
	$(CC) $(OPTIONS) -I$(INCLUDEDIR) $(MAIN)/main.cpp $(OBJ) -o main
	g++ main.cpp -o main -g -std=c++2a -Wall -pedantic -o3

$(LIBDIR)/%.o : $(LIBDIR)/%.cpp $(INCLUDEDIR)/%.h
	$(CC) $(OPTIONS) -c -I$(INCLUDEDIR) -o $@ $<

clean:
	rm -rf $(OBJ) main