
FLAGS=-c -g -I$(BULL) 
#-Wfatal-errors

LIB=lib/libbullet.a

all: test

include bullet.mk
	
obj/capi.o: capi.cpp
	g++ $(FLAGS) capi.cpp -o obj/capi.o

test: obj/test.o lib/libbullet.a obj/capi.o
	g++  obj/capi.o obj/test.o $(LIB) -o test

obj/test.o: test.c
	gcc -std=c11 $(FLAGS) test.c -o obj/test.o

cleanall: clean		
	rm -rf obj/*.o
	rm -rf lib/libbullet.a
	
clean:
	rm -rf test
	rm -rf obj/test.o
	rm -rf obj/capi.o
	

