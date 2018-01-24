
CFLAGS=-Ofast -c -Iinclude -I../../kazmath/kazmath/ -Wall
CFLAGS+= -I../capi/ -I../../bullet3/src/
LDFLAGS=-lGL -lm -lglfw ../capi/lib/libbullet.a

SRCmain=$(wildcard src/*.c)
SRCkaz=$(wildcard ../../kazmath/kazmath/*.c)
SRC=$(SRCmain) $(SRCkaz)
OBJ=$(SRCmain:src/%.c=obj/%.o)
OBJkaz=$(SRCkaz:../../kazmath/kazmath/%.c=obj/kaz/%.o)

CC=gcc

main: $(OBJ) $(OBJkaz) obj/capi.o
	g++ obj/capi.o $(OBJ) $(OBJkaz) -o main $(LDFLAGS)


$(OBJ): obj/%.o : src/%.c
	$(CC) -std=c99 $(CFLAGS) -c $< -o $@

$(OBJkaz): obj/kaz/%.o : ../../kazmath/kazmath/%.c
	$(CC)  -std=c99 -c $(CFLAGS) -c $< -o $@

obj/capi.o: ../capi/capi.cpp
	g++ -c $(CFLAGS) ../capi/capi.cpp -o obj/capi.o
	
check: main
	valgrind --gen-suppressions=all --suppressions=valgind.suppression.txt --track-origins=yes --leak-check=full ./main

clean:
	rm -rf main
	rm -rf obj/*.o
	rm -rf obj/kaz/*.o
	rm -rf obj/capi.o


