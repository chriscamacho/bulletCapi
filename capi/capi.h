/// note to self, use camel case, because such irrelevancies, upsets reddit

#ifndef CAPI_H
#define CAPI_H

// Vec has 4 components so it can be used for quaternions too...
typedef struct {
	float x,y,z,w;
} Vec;

// sphere = 8
// box = 0
// cylinder = 13

typedef enum { T_SPHERE = 8, T_BOX=0, T_CYLINDER=13 } ShapeType;

void* createUniverse();
void destroyUniverse(void* uni); /// muhahahaha
void setGravity(void* uni, float x, float y, float z);
void stepWorld(void* u, float dt, int i);
void collisionCallback(void* u, void(*callback)(void*, void*, const Vec*, const Vec*, const Vec*) );
///								void contact(void* b1, void* b2, const Vec* ptA, const Vec* ptB, const Vec* norm)

void* createBoxShape(void* uni, float ex, float ey, float ez);
void* createSphereShape(void* u, float re);
void* createCylinderShape(void* u,  float x, float y);


void* createBody(void* u, void* shape, float mass, float x, float y, float z);
void bodyGetPosition(void* body, Vec* pos );
void bodyGetOrientation(void* body, Vec* r);
void bodyGetPositionAndOrientation(void* body, Vec* pos, Vec* r);
void bodyGetOpenGLMatrix(void* body, float* m);
void bodyApplyImpulse(void* body, Vec* i, Vec* p);
void bodyApplyTorque(void* body, Vec* t);
void bodyGetLinearVelocity(void* body, Vec* v);
void bodySetLinearVelocity(void* body, Vec v);
void bodyGetAngularVelocity(void* body, Vec* v);
void bodySetRotation(void* body, float yaw, float pitch, float roll);
void bodySetRestitution(void* body, float r);
void bodySetFriction(void* s, float f);
float bodyGetFriction(void* s);

// should only be used when you need no "teleport" an object not move it
void bodySetPosition(void* body, Vec pos );

int bodyGetShapeType(void* body);







#endif




