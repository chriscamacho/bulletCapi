
/** @file capi.h
 * @brief a C wrapper for the bullet physics engine
 * @author Chris Camacho (codifies)
 * @author http://bedroomcoders.co.uk/captcha/
 */

// note to self, use camel case, because such irrelevancies, upsets reddit


#ifndef CAPI_H
#define CAPI_H

#include <stdbool.h>

#define PI 3.14159265359

/** DEACTIVATION_ENABLE enable deactivation used with bodySetDeactivation*/
#define DEACTIVATION_ENABLE 3
/** DEACTIVATION_DISABLE disable deactivation used with bodySetDeactivation*/
#define DEACTIVATION_DISABLE 4

// Vec has 4 components so it can be used for quaternions too...
/** struct to hold vector or quaternions */
typedef struct {
	float x,y,z,w;
} Vec;

// copied from btBroadphaseProxy.h so C app doesn't have to import
// bullet c++ headers
//
// TODO !!! find some better way!
// 
// NB will need to recopy this if it changes in bullet - not ideal...
// have to rename enums so no duplicate definition for C++ "wrapper"
/** \cond HIDDEN */
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
/** \endcond */

/** types used to distinguish shape types 
 *
	@param T_SPHERE sphere type
	@param T_BOX box type
	@param T_CYLINDER cylinder type
	@param T_COMPOUND compound type
*/
typedef enum { 
	T_SPHERE		=	xSPHERE_SHAPE_PROXYTYPE,
	T_BOX			=	xBOX_SHAPE_PROXYTYPE,
	T_CYLINDER		=	xCYLINDER_SHAPE_PROXYTYPE,
	T_COMPOUND		=	xCOMPOUND_SHAPE_PROXYTYPE
} ShapeType;

// See TODO re: BroadphaseNativeTypesCOPY
/** values used to set constraint params 
 * @param C_ERP
 * @param C_STOP_ERP
 * @param C_CFM
 * @param C_STOP_CFM
 * */
typedef enum  {
	C_ERP=1,		//BT_CONSTRAINT_ERP=1,	
	C_STOP_ERP,		//BT_CONSTRAINT_STOP_ERP,
	C_CFM,			//BT_CONSTRAINT_CFM,
	C_STOP_CFM		//BT_CONSTRAINT_STOP_CFM
} ConstraintParams;

/** creates a physics environment 
 * @return pointer to the environment 
 */
void* createUniverse();
/** releases the environment
 * @param uni pointer to the previously created environment
 */
void destroyUniverse(void* uni); /// muhahahaha

/** sets the environments gravity */
void setGravity(void* uni, float x, float y, float z);
/** step the world
 * @param u the environment to step
 * @param dt the amount of time to step
 * @param i number of iterations 
 */
void stepWorld(void* u, float dt, int i);

/** set the collision callback
 * @param u the universe
 * @param callback function pointer to the contact callback
 * 
 * void contact(void* b1, void* b2, const Vec* ptA, const Vec* ptB, const Vec* norm)
 * @param b1 body A
 * @param b2 body B
 * @param ptA point in body A of the contact
 * @param ptB point in body B of the contact
 * @param norm the collision normal
 */
void collisionCallback(void* u, void(*callback)(void*, void*, const Vec*, const Vec*, const Vec*) );

/** creat a box shape
 * @param uni the universe that the shape is intended for
 * 			this is for tidying up when the universe is destroyed
 * @param ex X extent of the box
 * @param ey Y extent of the box
 * @param ez Z extent of the box
 */ 
void* createBoxShape(void* uni, float ex, float ey, float ez);

/** creates a sphere shape
 * @param u the universe that releases this shape
 * @param re radius of sphere
 */
void* createSphereShape(void* u, float re);

/** creates a cylinder shape, length along the Z axis
 * @param u the universe that releases this shape
 * @param r the radius of the cylinder
 * @param l the length of the cylinder
 */
void* createCylinderShapeZ(void* u,  float r, float l);

/** creates a cylinder shape, length along the Y axis
 * @param u the universe that releases this shape
 * @param r the radius of the cylinder
 * @param l the length of the cylinder
 */
void* createCylinderShapeY(void* u,  float r, float l);

/** creates a cylinder shape, length along the X axis
 * @param u the universe that releases this shape
 * @param r the radius of the cylinder
 * @param l the length of the cylinder
 */
void* createCylinderShapeX(void* u,  float r, float l);

/** create a compound shape
 * @param u the universe that releases this shape
 * 
 * The compound shape is a "empty" shape you add other shapes
 * to to make a single shape from.
 */
void* createCompoundShape(void* u);

/** adds other primatives to the compound shape
 * @param compound pointer to the compound shape
 * @param child pointer to the child shape
 * @param x,y,z local position of shape in the compound
 * @param yaw,pitch,roll local rotation in the compound
 */
void addCompoundChild(void* compound, void* child, float x, float y, float z,
						float yaw, float pitch, float roll);

/** create a body
 * @param u the universe or environment the body is in
 * @param shape pointer to the shape to create the body with
 * @param mass mass of the body
 * @param x,y,z position to start the body from
 */
void* createBody(void* u, void* shape, float mass, float x, float y, float z);

/** get the bodies position
 * @param body
 * @param pos the supplied structured is filled in with the position
 */
void bodyGetPosition(void* body, Vec* pos );

/** get body orientation
 * @param body
 * @param r rotation returned in this Vec
 */
void bodyGetOrientation(void* body, Vec* r);

/** get the position and orientation of a body
 * @param body
 * @param pos the position
 * @param r the rotation
 */
void bodyGetPositionAndOrientation(void* body, Vec* pos, Vec* r);

/** get a matrix representing the orientaion and position of a body
 * @param body
 * @param m the pointer to a matrix struct (16 floats) can be directly
 * used by OpenGL
 */
void bodyGetOpenGLMatrix(void* body, float* m);

/** apply an impulse to a body
 * @param body
 * @param i impulse vector
 * @param p where on the body to apply the impulse
 */
void bodyApplyImpulse(void* body, Vec* i, Vec* p);

/** apply a rotational force to the body
 * @param body
 * @param t torque vector
 */
void bodyApplyTorque(void* body, Vec* t);

/** get the linear velocity of a body
 * @param body
 * @param v pointer to a Vec for the velocity
 */
void bodyGetLinearVelocity(void* body, Vec* v);

/** set the velocity of a body - 
 * this should not be used every frame rather this should be used one
 * off in special circumstances like teleportation.
 * @param body
 * @param v the velocity
 */
void bodySetLinearVelocity(void* body, Vec v);

/** get the angular velocity
 * @param body
 * @param v the velocity
 */
void bodyGetAngularVelocity(void* body, Vec* v);

/** set angluar velocity - see notes in bodySetLinearVelocity
 * @param body
 * @param v velocity
 */
void bodySetAngularVelocity(void* body, Vec v);

/** set the rotation of the body (in radians)
 * @param body
 * @param pitch,yaw,roll rotations around the x,y & z axis respectivly
 */
void bodySetRotationEular(void* body, float pitch, float yaw, float roll);

/** set the rotation of a body as bodySetRotationEular except using a Vec
 * @param body
 * @param r the rotation
 */
 
/** sets a bodies rotation - see warning in bodySetLinearVelocity
 * @param body
 * @param r rotation
 */
void bodySetRotation(void* body, Vec r);

/** set the restitution of a body (bouncy-ness)
 * @param body
 * @param r restitution
 */
void bodySetRestitution(void* body, float r);

/** set bodies friction
 * @param s the body
 * @param f friction
 */ 
void bodySetFriction(void* s, float f);

/** get the bodies friction setting
 * @param s body
 * @return friction setting
 */
float bodyGetFriction(void* s);

/** set the bodies position - see notes in bodySetLinearVelocity
 * @param body
 * @param pos the position
 */
void bodySetPosition(void* body, Vec pos );

/** get the bodies shape type - see ShapeType enum
 * @param body
 */
int bodyGetShapeType(void* body);

/** set body deactivation
 * @param b body
 * @param v true if body should be deactivated after a period without collisions
 */
void bodySetDeactivation(void* b, bool v);



void* createHinge2Constraint(void* u, void* bodyA, void* bodyB, Vec anchor,
								Vec parentAxis, Vec childAxis);
void hinge2setLowerLimit(void* h, float l);
void hinge2setUpperLimit(void* h, float l);


/** creates a hinge constraint (joint)
 * @param uni the universe the joint should be in
 * @param bodyA,bodyB the two bodies involved
 * @param pivA,rotA the pivot point and rotation of the hinge axis
 * @param pivB,rotB pivot and rotation relative to bodyB
 * @param refA use reference frame A or not
 * @param collide should these bodies collide or not
 */
void* createHingeConstraint(void* uni, void* bodyA, void* bodyB, 
					Vec pivA, Vec rotA, 
					Vec pivB, Vec rotB, bool refA, bool collide);

/** set the hinge angluar limit
 * @param hinge pointer to the joint
 * @param low set the lower limit  < -PI is unlimited
 * @param hi set the upper limit  > PI is unlimited
 */
void hingeSetLimit(void* hinge, float low, float hi);

/** enable rotational motor for a hinge
 * @param hinge pointer to the constraint
 * @param enableMotor true enable
 * @param targetVelocity the motor will always try to achive this velocity
 * @param maxMotorImpulse limits the impulse the motor can use
 */
void hingeEnableAngularMotor(void* hinge, bool enableMotor, float targetVelocity,
								float maxMotorImpulse);

//float hingeGetAngle(void* hinge);

/** sets a constrains parameters
 * @param c constraint pointer
 * @param num see ConstraintParams enum
 * @param value
 * @param axis 0-5 but see implementation as some are handled differently
 * for example hinge only uses axis 5 (which can be refered to as -1)
 */
void setConstraintParam(void* c, int num, float value, int axis);

/** is a constraint enabled
 * @param c constraint pointer
 */
bool isConstraintEnabled(void* c);

/** enable or disable a constraint
 * @param c constraint pointer
 * @param en true or false to enable / disable
 */ 
void setConstraintEnabled(void* c, bool en);


/** remove body from the universe
 * @param u universe pointer
 * @param b body
 */
void removeBody(void* u, void* b);

/** delete a shape
 * @param u universe pointer
 * @param s shape
 */ 
void deleteShape(void* u, void* s);

/** delete a body (release its resources)
 * @param b body
 */
void deleteBody(void* b);

/** get the shape attached to the body
 * @param b body
 */
void* bodyGetShape(void* b);

/** number of shapes in a compound
 * @param s shape
 */
int compoundGetNumChildren(void* s);

/** remove a shape from a compound shape
 * @param s shape
 * @param index
 */
void compoundRemoveShape(void* s, int index);

#endif




