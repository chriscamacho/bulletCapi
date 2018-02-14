/// note to self, use camel case, because such irrelevancies, upsets reddit

#ifndef CAPI_H
#define CAPI_H

#include <stdbool.h>

// Vec has 4 components so it can be used for quaternions too...
typedef struct {
	float x,y,z,w;
} Vec;

// copyed from btBroadphaseProxy.h so C app doesn't have to import
// bullet c++ headers
//
// TODO !!! find some better way!
// 
// NB will need to recopy this if it changes in bullet - not ideal...
// have to rename enums so no duplicate definition for C++ "wrapper"

enum BroadphaseNativeTypesCOPY
{
	// polyhedral convex shapes
	xBOX_SHAPE_PROXYTYPE,
	xTRIANGLE_SHAPE_PROXYTYPE,
	xTETRAHEDRAL_SHAPE_PROXYTYPE,
	xCONVEX_TRIANGLEMESH_SHAPE_PROXYTYPE,
	xCONVEX_HULL_SHAPE_PROXYTYPE,
	xCONVEX_POINT_CLOUD_SHAPE_PROXYTYPE,
	xCUSTOM_POLYHEDRAL_SHAPE_TYPE,
//implicit convex shapes
xIMPLICIT_CONVEX_SHAPES_START_HERE,
	xSPHERE_SHAPE_PROXYTYPE,
	xMULTI_SPHERE_SHAPE_PROXYTYPE,
	xCAPSULE_SHAPE_PROXYTYPE,
	xCONE_SHAPE_PROXYTYPE,
	xCONVEX_SHAPE_PROXYTYPE,
	xCYLINDER_SHAPE_PROXYTYPE,
	xUNIFORM_SCALING_SHAPE_PROXYTYPE,
	xMINKOWSKI_SUM_SHAPE_PROXYTYPE,
	xMINKOWSKI_DIFFERENCE_SHAPE_PROXYTYPE,
	xBOX_2D_SHAPE_PROXYTYPE,
	xCONVEX_2D_SHAPE_PROXYTYPE,
	xCUSTOM_CONVEX_SHAPE_TYPE,
//concave shapes
xCONCAVE_SHAPES_START_HERE,
	//keep all the convex shapetype below here, for the check IsConvexShape in broadphase proxy!
	xTRIANGLE_MESH_SHAPE_PROXYTYPE,
	xSCALED_TRIANGLE_MESH_SHAPE_PROXYTYPE,
	///used for demo integration FAST/Swift collision library and Bullet
	xFAST_CONCAVE_MESH_PROXYTYPE,
	//terrain
	xTERRAIN_SHAPE_PROXYTYPE,
///Used for GIMPACT Trimesh integration
	xGIMPACT_SHAPE_PROXYTYPE,
///Multimaterial mesh
    xMULTIMATERIAL_TRIANGLE_MESH_PROXYTYPE,
	
	xEMPTY_SHAPE_PROXYTYPE,
	xSTATIC_PLANE_PROXYTYPE,
	xCUSTOM_CONCAVE_SHAPE_TYPE,
xCONCAVE_SHAPES_END_HERE,

	xCOMPOUND_SHAPE_PROXYTYPE,

	xSOFTBODY_SHAPE_PROXYTYPE,
	xHFFLUID_SHAPE_PROXYTYPE,
	xHFFLUID_BUOYANT_CONVEX_SHAPE_PROXYTYPE,
	xINVALID_SHAPE_PROXYTYPE,

	xMAX_BROADPHASE_COLLISION_TYPES
	
};


typedef enum { 
	T_SPHERE		=	xSPHERE_SHAPE_PROXYTYPE,
	T_BOX			=	xBOX_SHAPE_PROXYTYPE,
	T_CYLINDER		=	xCYLINDER_SHAPE_PROXYTYPE,
	T_COMPOUND		=	xCOMPOUND_SHAPE_PROXYTYPE
} ShapeType;


void* createUniverse();
void destroyUniverse(void* uni); /// muhahahaha
void setGravity(void* uni, float x, float y, float z);
void stepWorld(void* u, float dt, int i);
void collisionCallback(void* u, void(*callback)(void*, void*, const Vec*, const Vec*, const Vec*) );
///								void contact(void* b1, void* b2, const Vec* ptA, const Vec* ptB, const Vec* norm)

void* createBoxShape(void* uni, float ex, float ey, float ez);
void* createSphereShape(void* u, float re);
void* createCylinderShape(void* u,  float x, float y);
void* createCompoundShape(void* u);
void addCompoundChild(void* compound, void* child, float x, float y, float z,
						float yaw, float pitch, float roll);

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
void bodySetAngularVelocity(void* body, Vec v);
void bodySetRotationEular(void* body, float pitch, float yaw, float roll);
void bodySetRotation(void* body, Vec r);
void bodySetRestitution(void* body, float r);
void bodySetFriction(void* s, float f);
float bodyGetFriction(void* s);

// should only be used when you need to "teleport" an object not move it
void bodySetPosition(void* body, Vec pos );

int bodyGetShapeType(void* body);

/* TODO started adding this thinking it was a normal hinge
 * needs a few extra support functions for editing properties
 * seems to be working... 
 * 
void* createHinge2Constraint(void* u, void* bodyA, void* bodyB, Vec anchor,
								Vec parentAxis, Vec childAxis);
void hinge2setLowerLimit(void* h, float l);
void hinge2setUpperLimit(void* h, float l);
*/
void* createHinge(void* uni, void* bodyA, void* bodyB, 
					Vec pivA, Vec rotA, 
					Vec pivB, Vec rotB, bool refA, bool collide);

void hingeSetLimit(void* hinge, float low, float hi);
void hingeEnableAngularMotor(void* hinge, bool enableMotor, float targetVelocity,
								float maxMotorImpulse);



#endif




