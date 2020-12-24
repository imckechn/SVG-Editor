CFLAGS = -Wall -g -std=c11
BIN = bin/
INC = include/
SRC = src/
XML_PATH = /usr/include/libxml2

# Standard common makefile
parser: $(BIN)libsvgparse.so

$(BIN)libsvgparse.so: $(BIN)SVGParser.o $(BIN)LinkedListAPI.o
	gcc -shared -o $(BIN)libsvgparse.so $(BIN)SVGParser.o $(BIN)LinkedListAPI.o -lxml2 -lm

$(BIN)SVGParser.o: $(SRC)SVGParser.c $(INC)LinkedListAPI.h $(INC)SVGParser.h
	gcc -c -fpic $(CFLAGS) -I$(XML_PATH) -I$(INC) $(SRC)SVGParser.c -o $(BIN)SVGParser.o

$(BIN)LinkedListAPI.o: $(SRC)LinkedListAPI.c $(INC)LinkedListAPI.h
	gcc -c -fpic $(CFLAGS) -I$(INC) $(SRC)LinkedListAPI.c -o $(BIN)LinkedListAPI.o

main: parser
	gcc $(CFLAGS) -c $(SRC)main.c -I$(XML_PATH) -I$(INC) -o $(BIN)main.o
	gcc $(BIN)main.o -L$(BIN) -lsvgparse -o $(BIN)main
	export LD_LIBRARY_PATH=$$LD_LIBRARY_PATH:.:$(BIN); \
	./$(BIN)main

valgrind: parser
	gcc $(CFLAGS) -c $(SRC)main.c -I$(XML_PATH) -I$(INC) -o $(BIN)main.o
	gcc $(BIN)main.o -L$(BIN) -lsvgparse -o $(BIN)main
	export LD_LIBRARY_PATH=$$LD_LIBRARY_PATH:.:$(BIN); \
	valgrind --leak-check=full --show-leak-kinds=all ./$(BIN)main

clear: clean
	rm $(BIN)main

clean:
	rm $(BIN)*.o $(BIN)*.so
