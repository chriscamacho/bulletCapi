
FLAGS=-Ofast -c -I$(BULL) --fast-math
#-Wfatal-errors


all: test

include bullet.mk
	
capi.a: capi.cpp
	g++ $(FLAGS) capi.cpp -o obj/capi.o
	ar rcs capi.a obj/capi.o

test: obj/test.o capi.a
	g++ obj/test.o capi.a lib/libbullet.a -o test

obj/test.o: test.c
	gcc -std=c11 $(FLAGS) test.c -o obj/test.o

cleanall: clean		
	rm -rf obj/*.o
	rm -rf lib/libbullet.a
	
clean:
	rm -rf test
	rm -rf obj/test.o
	rm -rf obj/capi.o
	rm -rf capi.a
	
	

