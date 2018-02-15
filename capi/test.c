#include "stdio.h"
#include "capi.h"

void* groundBody;
void* fallingBody;

void contact(void* b1, void* b2, const Vec* ptA, const Vec* ptB, const Vec* norm) {
	printf("------\n");
	if (b1==groundBody) printf("body1 == ground, ");
	if (b1==fallingBody) printf("body1 == falling body, ");
	if (b2==groundBody) printf("body2 == ground\n");
	if (b2==fallingBody) printf("body2 == falling body\n");
	printf("ptA %2.4lf %2.4lf %2.4lf ", ptA->x, ptA->y, ptA->z);
	printf("ptB %2.4lf %2.4lf %2.4lf ", ptB->x, ptB->y, ptB->z);
	printf("Nrm %2.4lf %2.4lf %2.4lf\n", norm->x, norm->y, norm->z);
	printf("------\n");
}

int main(int argc, char* argv[]) {

	void* uni = createUniverse();
	
	setGravity(uni, 0, 0, -9.98);
	
	void* groundShape = createBoxShape(uni, 100, 100, 5);	// size 100,100,5
	groundBody = createBody(uni, groundShape, 0, 0, 0, -5);	// 0 mass == static pos 0,0,-5
	bodySetRestitution(groundBody, .9);
	
	//void* fallingShape = createSphereShape(uni, 1.);
	//void* fallingShape = createBoxShape(uni, .5, .5, .5);
	void* fallingShape = createCylinderShape(uni, .5, 1);
	
	fallingBody = createBody(uni, fallingShape, 10, 0, 0, 1.01);
	printf("shape type = %i\n",bodyGetShapeType(fallingBody));
	bodySetRestitution(fallingBody, .9);
	printf("ball friction was %lf\n",bodyGetFriction(fallingBody));
	bodySetFriction(fallingBody, 2.0); // just testing....
	printf("ball friction now %lf\n",bodyGetFriction(fallingBody));
	
	// apply impulse to make it more interesting...
	Vec pos,imp;
	pos.x=0; pos.y=0; pos.z=0;
	imp.x=0.1; imp.y=0.12; imp.z=0;	
	bodyApplyImpulse(fallingBody, &imp, &pos);

	for (int i = 0; i < 18; i++) {
		stepWorld(uni, 1./120., 8);
		
		collisionCallback(uni, &contact );
		
		/*
		bodyGetPosition(fallingBody, &pos);
		printf("step %i  %2.4lf %2.4lf %2.4lf ", i, pos.x, pos.y, pos.z);
		bodyGetOrientation(fallingBody, &pos);
		printf(" r=%2.4lf %2.4lf %2.4lf %2.4lf  ", pos.x, pos.y, pos.z, pos.w);
		*/
		
		bodyGetPositionAndOrientation(fallingBody, &pos, &imp);
		printf("step %i  %2.4lf %2.4lf %2.4lf ", i, pos.x, pos.y, pos.z);
		printf(" r=%2.4lf %2.4lf %2.4lf %2.4lf\n", imp.x, imp.y, imp.z, imp.w);
		
		bodyGetLinearVelocity(fallingBody, &pos);
		printf(" v=%2.4lf %2.4lf %2.4lf \n", pos.x, pos.y, pos.z);
	}	

	destroyUniverse(uni);

}
