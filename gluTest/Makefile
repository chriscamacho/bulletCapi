

FLAGS=-c -g -I../capi/ -I../../bullet3/src/
#-Wfatal-errors

LIB=../capi/lib/libbullet.a -lGL -lglut -lGLU 


all: main
	
obj/capi.o: ../capi/capi.cpp
	g++ $(FLAGS) ../capi/capi.cpp -o obj/capi.o

main: obj/main.o obj/capi.o
	g++  obj/capi.o obj/main.o $(LIB) -o main

obj/main.o: main.c
	gcc -std=c11 $(FLAGS) main.c -o obj/main.o

check: main
	valgrind --suppressions=valgind.suppression.txt --leak-check=full ./main
clean:
	rm -rf main
	rm -rf obj/main.o
	rm -rf obj/capi.o
	

