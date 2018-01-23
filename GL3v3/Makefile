#FLAGS=-c -g -I../capi/ -I../../bullet3/src/
#-Wfatal-errors

#LIB=../capi/lib/libbullet.a -lGL -lglut -lGLU 


#all: main
	
#obj/capi.o: ../capi/capi.cpp
#	g++ $(FLAGS) ../capi/capi.cpp -o obj/capi.o

#main: obj/main.o obj/capi.o
#	g++  obj/capi.o obj/main.o $(LIB) -o main

#obj/main.o: main.c
#	gcc -std=c11 $(FLAGS) main.c -o obj/main.o

#check: main
#	valgrind --suppressions=valgind.suppression.txt --leak-check=full ./main
#clean:
#	rm -rf main
#	rm -rf obj/main.o
#	rm -rf obj/capi.o
	
#--------------------------------


CFLAGS=-c -g -Iinclude -I../../kazmath/kazmath/ -Wall
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
	valgrind --suppressions=valgind.suppression.txt --leak-check=full ./main

clean:
	rm -rf main
	rm -rf obj/*.o
	rm -rf obj/kaz/*.o
	rm -rf obj/capi.o


