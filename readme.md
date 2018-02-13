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
is very tiny at the moment but it is grouped into a hopefully easy to
use set of functions

### Universe API
These routines effect the global properties of the simulation and 
provide a pointer that a lot of other functions require

#### ``void *createUniverse();``
Sets up the simulation environment and returns a pointer to that
environment

#### ``void destroyUniverse(void* uni); /// muhahahaha``
Don't use if 007 is around, basically frees all resources (including
bodies and shapes) and all other resources used by the simulation.

#### ``void setGravity(void* uni, float x, float y, float z);``
You're not forced to have +tive Z or Y as "up" set the global direction
of gravity to what makes sense for your setup

#### ``void* stepWorld(void* u, float dt, int i);``
This "steps" the simulation dt is a decimal fraction of a second and
i is the number of iterations to use

#### ``void collisionCallback(void* u, void(*callback)(void*, void*, const Vec*, const Vec*, const Vec*) );``
call a callback function for each object pair that has collided this step

##### ``void contact(void* b1, void* b2, const Vec* ptA, const Vec* ptB, const Vec* norm)``
the prototype for the callback function b1 & b2 are pointers to the two bodies that collided, ptA & ptB are the
locations of the collisions, norm is the collision normal with reference
to the second body (b2 or B) 


### The shape API
more later! for now these basic shapes are enough for testing and basic
demostration

#### ``void* createBoxShape(void* uni, float ex, float ey, float ez);``
pass the "universe" pointer ex,ey & ez define the extent of the box

#### ``void* createSphereShape(void* u, float re);``
here re specifies the radial extent of the sphere.

#### ``void* createCylinderShape(void* u,  float x, float y);``
cylinder shape x is radius, y is length

#### ``void* createCompoundShape(void* u);``
creates a new compound shape

#### ``void addCompoundChild(void* compound, void* child, float x, float y, float z, float yaw, float pitch, float roll);``

adds a shape to a compound shape, allowing you to position and rotate it 
relative to the origin of the compound shape.

### The body API
These function are used to create and manipulate physics bodies

#### ``void* createBody(void* u, void* shape, float mass, float x, float y, float z);``
creates a body using the specified shape, a zero mass represents a 
static body.  X, Y & Z define the start position

#### ``void bodyGetPosition(void* body, Vec* pos );``
provide a pointer to the body and a Vec structure, the Vec is set with
the position of the body

#### ``void bodyGetOrientation(void* body, Vec* r);``
fills in r with a quaternion representing the body orientation

#### ``void bodyGetPositionAndOrientation(void* body, Vec* pos, Vec* r);``
get both the position and orientation

#### ``void bodyGetOpenGLMatrix(void* body, float* m);``
get the position and orientation represented by a 16 float array 
suitable for use as an OpenGL Matrix

#### ``void bodySetRotation(void* body, float yaw, float pitch, float roll);``
intended only to be used to set an initial rotation of a body, y,p & r are in
radians 0-2pi

#### ``void bodyApplyTorque(void* body, Vec* t);``
adds rotational force (torque) to the body

#### ``void bodyApplyImpulse(void* body, Vec* i, Vec* p);``
apply an impulse or push i vector, from the bodies position p

#### ``void bodyGetLinearVelocity(void* body, Vec* v);``
get a vector representing the linear velocity of a body

#### ``void bodyGetAngularVelocity(void* body, Vec* v);``
gets the angular or rotational velocity

#### ``void bodySetRestitution(void* body, float r);``
set the restitution (bounciness) of a body

#### ``void* bodySetFriction(void* s, float f);``
set the friction for the body

#### ``float bodyGetFriction(void* s);``
I only wrapped this to check the set friction was working as expected
(hard to tell from a console application!) probably of limited use

#### ``int bodyGetShapeType(void* body);``
gets the shape type attached to the body
	T_SPHERE, T_BOX, T_CYLINDER

### Conclusion
This is very early days, there is lots to do, any contributions 
especially examples most apreciated, it an example needs some missing
functionallity please do post an issue.
