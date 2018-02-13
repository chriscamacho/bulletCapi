#include "capi.hpp"
#include "btBulletDynamicsCommon.h"

void* createUniverse() {
	universe* u = (universe*)malloc(sizeof(universe));
	u->collisionConfiguration = new btDefaultCollisionConfiguration();
	u->dispatcher = new btCollisionDispatcher(u->collisionConfiguration);
	u->broadphase = new btDbvtBroadphase();
	u->solver = new btSequentialImpulseConstraintSolver;
	u->dynamicsWorld = new btDiscreteDynamicsWorld(u->dispatcher, u->broadphase, u->solver, u->collisionConfiguration);

	// possibly could have an iteration callback function so all shapes in the
	// world get passed one at a time to a callback function(s)
	// only used for clean up for now...
	u->collisionShapes =  new btAlignedObjectArray<btCollisionShape*>();
	
	//float a;
	//btScalar b;
	//printf("sizeof float %i sizeof btScalar %i\n", sizeof(a),sizeof(b));

	return (void*)u;
}


void destroyUniverse(void* u) {
	
	for (int i = UNI(u)->dynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i--)
	{
		btCollisionObject* obj = UNI(u)->dynamicsWorld->getCollisionObjectArray().at(i);
		btRigidBody* body = btRigidBody::upcast(obj);
		if (body && body->getMotionState())
		{
			delete body->getMotionState();
		}
		UNI(u)->dynamicsWorld->removeCollisionObject(obj);
		delete obj;
	}

	for (int j = 0; j < UNI(u)->collisionShapes->size(); j++)
	{
		btCollisionShape* shape = (btCollisionShape*)(UNI(u)->collisionShapes->at(j));
		//collisionShapes[j] = 0;
		delete shape;
	}
	
	delete UNI(u)->dynamicsWorld;
	delete UNI(u)->solver;
	delete UNI(u)->broadphase;
	delete UNI(u)->dispatcher;
	delete UNI(u)->collisionConfiguration;
	UNI(u)->collisionShapes->clear();
	delete UNI(u)->collisionShapes;
	
	free(u);
}

void setGravity(void* u, float x, float y, float z) {
	UNI(u)->dynamicsWorld->setGravity(btVector3(x,y,z));
}

void* createBoxShape(void* u, float ex, float ey, float ez) {
	btCollisionShape* shape = new btBoxShape(btVector3(btScalar(ex), btScalar(ey), btScalar(ez)));
	UNI(u)->collisionShapes->push_back(shape);
	return (void*)shape;
}

void* createCompoundShape(void* u) {
	btCollisionShape* shape = new btCompoundShape();
	UNI(u)->collisionShapes->push_back(shape);
	return (void*)shape;
}

void addCompoundChild(void* compound, void* child, float x, float y, float z,
						float yaw, float pitch, float roll) {
	
	btTransform localTrans;
	localTrans.setIdentity();
	localTrans.setOrigin(btVector3(x,y,z));
	
	btQuaternion quat;
	quat.setEuler(yaw, pitch, roll);
	localTrans.setRotation(quat);

	((btCompoundShape*)compound)->addChildShape(localTrans,SHAPE(child));
}

void* createSphereShape(void* u, float re) {
	btCollisionShape* shape = new btSphereShape(re);
	UNI(u)->collisionShapes->push_back(shape);
	return (void*)shape;
}

void* createCylinderShape(void* u,  float x, float y) {
	btCollisionShape* shape = new btCylinderShapeZ(btVector3(x,x,y));
	UNI(u)->collisionShapes->push_back(shape);
	return (void*)shape;	
}

void* createBody(void* u, void* shape, float mass, float x, float y, float z) {
	// heavily "influenced" from bullet manual hello world console example
	btTransform trans;
	trans.setIdentity();
	trans.setOrigin(btVector3(x, y, z));
	
	btScalar M(mass);
	bool isDynamic = (M != 0.f);

	btVector3 localInertia(0, 0, 0);
	if (isDynamic)
		SHAPE(shape)->calculateLocalInertia(M, localInertia);

	btDefaultMotionState* motionState = new btDefaultMotionState(trans);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(M, motionState, SHAPE(shape), localInertia);
	btRigidBody* body = new btRigidBody(rbInfo);

	UNI(u)->dynamicsWorld->addRigidBody(body);
	return body;
}

void stepWorld(void* u, float dt, int i) {
	UNI(u)->dynamicsWorld->stepSimulation(dt, i);
}

void bodyGetPositionAndOrientation(void* body, Vec* pos, Vec* r) {
	btTransform trans;
	//if (BODY(body) && BODY(body)->getMotionState())	{
	//	BODY(body)->getMotionState()->getWorldTransform(trans);
	//} else {
		trans = BODY(body)->getWorldTransform();
	//}
	pos->x = trans.getOrigin().getX();
	pos->y = trans.getOrigin().getY();
	pos->z = trans.getOrigin().getZ();	
	
	btQuaternion q = trans.getRotation();
	
	r->x = q.getX();
	r->y = q.getY();
	r->z = q.getZ();	
	r->w = q.getW();	
}

void bodyGetPosition(void* body, Vec* pos ) {
	btTransform trans;
	//if (BODY(body) && BODY(body)->getMotionState())	{
	//	BODY(body)->getMotionState()->getWorldTransform(trans);
	//} else {
		trans = BODY(body)->getWorldTransform();
	//}
	pos->x = trans.getOrigin().getX();
	pos->y = trans.getOrigin().getY();
	pos->z = trans.getOrigin().getZ();
}

void bodySetPosition(void* body, Vec pos ) {
	btTransform trans;
	trans.setOrigin(btVector3(pos.x,pos.y,pos.z));
	BODY(body)->setWorldTransform(trans);
}

void bodyGetOrientation(void* body, Vec* r) {
	btTransform trans;
	//if (BODY(body) && BODY(body)->getMotionState())	{
	//	BODY(body)->getMotionState()->getWorldTransform(trans);
	//} else {
		trans = BODY(body)->getWorldTransform();
	//}
	btQuaternion q = trans.getRotation();
	
	r->x = q.getX();
	r->y = q.getY();
	r->z = q.getZ();	
	r->w = q.getW();	
}

int bodyGetShapeType(void* body) {
	return BODY(body)->getCollisionShape()->getShapeType();
}

void bodyGetOpenGLMatrix(void* body, float* m) {
	btTransform trans;
	
	// TODO look into this...
	// commented only worked for dynamic bodies, getting world trans
	// direct from body works for both static and dynamic
	
//	if (BODY(body) && BODY(body)->getMotionState())	{
//		BODY(body)->getMotionState()->getWorldTransform(trans);
//	} else {
		trans = BODY(body)->getWorldTransform();
//	}
	trans.getOpenGLMatrix(m);
}

void bodyApplyImpulse(void* body, Vec* i, Vec* p) {
	BODY(body)->applyImpulse(btVector3(i->x,i->y,i->z), btVector3(p->x,p->y,p->z)); 	
}

void bodyApplyTorque(void* body, Vec* t) {
	BODY(body)->applyTorque(btVector3(t->x,t->y,t->z));
}

void bodySetRotationEular(void* body, float pitch, float yaw, float roll) {
	btQuaternion q = btQuaternion();
	q.setEuler(btScalar(yaw),btScalar(pitch),btScalar(roll));
	
	btTransform trans;

	trans = BODY(body)->getCenterOfMassTransform();
	trans.setRotation(q);
	
	BODY(body)->setCenterOfMassTransform(trans);
	
}

void bodySetRotation(void* body, Vec r) {
	bodySetRotationEular(body, r.x, r.y, r.z);
}

void bodySetRestitution(void* body, float r) {
	BODY(body)->setRestitution(r);
}

void bodyGetLinearVelocity(void* body, Vec* v) {
	const btVector3 bv = BODY(body)->getLinearVelocity();
	v->x = bv.getX();
	v->y = bv.getY();
	v->z = bv.getZ();
}

void bodySetLinearVelocity(void* body, Vec v) {
	BODY(body)->setLinearVelocity(btVector3(v.x,v.y,v.z));
}

void bodyGetAngularVelocity(void* body, Vec* v) {
	const btVector3 bv = BODY(body)->getAngularVelocity();
	v->x = bv.getX();
	v->y = bv.getY();
	v->z = bv.getZ();
}

void bodySetAngularVelocity(void* body, Vec v) {
	BODY(body)->setAngularVelocity(btVector3(v.x,v.y,v.z));
}

void bodySetFriction(void* s, float f) {
	BODY(s)->setFriction(f);
}

float bodyGetFriction(void* s) {
	return BODY(s)->getFriction();
}

void collisionCallback(void* u, void (*callback)(void*, void*, const Vec*, const Vec*, const Vec*) ) {
	int numManifolds = UNI(u)->dispatcher->getNumManifolds();
	
    for (int i = 0; i < numManifolds; i++) {
        btPersistentManifold* contactManifold = UNI(u)->dispatcher->getManifoldByIndexInternal(i);
        
        const btCollisionObject* obA = contactManifold->getBody0();
        const btCollisionObject* obB = contactManifold->getBody1();

        int numContacts = contactManifold->getNumContacts();
        for (int j = 0; j < numContacts; j++) {
            btManifoldPoint& pt = contactManifold->getContactPoint(j);
            if (pt.getDistance() < 0.f) {
                const btVector3& ptA = pt.getPositionWorldOnA();
                const btVector3& ptB = pt.getPositionWorldOnB();
                const btVector3& normalOnB = pt.m_normalWorldOnB;
                
                const Vec pa={ptA.getX(),ptA.getY(),ptA.getZ()};
                const Vec pb={ptB.getX(),ptB.getY(),ptB.getZ()};
                const Vec n={normalOnB.getX(),normalOnB.getY(),normalOnB.getZ()};
                callback((void*)obA, (void*)obB, &pa, &pb, &n);
            }
        }
    }
}
