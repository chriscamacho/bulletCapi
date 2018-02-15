### Why?

I wanted to use bullet physics from C (not a C++ fan)


### But doesn't bullet already have a C API?


Yes but alas its tightly enmeshed with the rest of the example code, 
like the exampleBrowser, I want to just use the core code of bullet I 
need and also provide my own graphics routines.

### So whats the point again?


The primary goal of this C API is to provide the core functionallity of
bullet without weighing it down with unrelated graphics or other utility
code

## Instructions
start off in the capi sub directory

compiling this project will provide you with a textual demonstration
and also a static library of the parts bullet the C API is currently 
using. In addition this is where the C header lives (capi.h)
In order for the makefile to work you will have to locate where you
have extracted the bullet sdk. Look inside bullet.mk

	# root of bullet source code
	BULL=../../bullet3/src/

adjust this path to point to the source directory of bullet, this part
of the build system is possibly less than optimal but it does ensure
complete independence from the bullet build system.

Once you have verified that the CAPI example works you can move onto the
simple graphical example.

alternativly if you are not bothered about playing with the console test
you can just do

    make lib/libbullet.a

in addition to libbullet.a you will need capi.o (compiled from capi.cpp)

### gluTest
This is a horror, no truly, its NOT an example of how you should produce
graphics.  That said its a minimum amount of code that can show some
kind of 3d graphics, without there being the confusion of a mini 
graphics engine, with model loaders, shaders and lots of other things
that could enmesh it into a very specific use case...
This example does show using some basic modifications to a body such as
changing basic properties like friction and restitution to make a 
pleasing demonstration

external to this folder gluTest links in libbullet.a and compiles
capi.cpp and links that to main.o the C frontend of this example.

### GL3v3
while a better (though probably not ideal) example of producing 3d graphics
this does add a bunch of extra clutter (theres way more OpenGL and support
stuff than actual bullet use!) This example is where new features are introduced
you will need to adjust the makefile for its dependancy (https://github.com/Kazade/kazmath)
if you clone it to a location alongside bulletCapi you should be good to go


### cppHello
This is a simple cpp reference example, building this with libraries
compiled by bullets own build system lead to difficult to diagnose 
memory corruption (hence I decided to implement bullet.mk...) it was
initially used to provide a reference design that I used to begin 
writing capi.cpp and capi.hpp which constitutes the C++ layer between
the C front end and the C++ backend
This will be removed before long as the "wrapper" is already more fully
featured that this example.



## The API

please see the doxygen docs




### Conclusion
This is very early days, there is lots to do, any contributions 
especially examples most apreciated, if an example needs some missing
functionallity please do post an issue.
