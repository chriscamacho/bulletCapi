/// note to self, use camel case, 'cause such irrelevancies, are just *loved* on reddit

#include "btBulletDynamicsCommon.h"

// C-api only sees this as a void pointer like bullet class pointers
/** \cond HIDDEN */
typedef struct {
	btDefaultCollisionConfiguration* collisionConfiguration;
	btCollisionDispatcher* dispatcher;
	btBroadphaseInterface* broadphase;
	btSequentialImpulseConstraintSolver* solver;
	btDiscreteDynamicsWorld* dynamicsWorld;
	btAlignedObjectArray<btCollisionShape*> *collisionShapes;
} universe;
/** \endcond */
// to ease casting a void*
#define UNI(x) ((universe*)x)
#define SHAPE(x) ((btCollisionShape*)x)
#define BODY(x) ((btRigidBody*)x)
#define JOINT(x) ((btTypedConstraint*)x)


extern "C" {
	
	#include "capi.h"

}







