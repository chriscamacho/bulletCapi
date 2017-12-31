/* 
 
 only used glew / glut for speed - really sucky example DONT DO THIS !!!
 that said it shows the basics of using bullet with your own graphics
 routines....

*/
#include <GL/glew.h>
#include <GL/glut.h>
#include "stdio.h"

#include "../capi/capi.h"

int keys[256]; 
float mat[16];
int* keyStates = &keys[0];
int win;

void* uni;
void* fallingBodies[16];


void keyOperations (void) {
    if (keyStates[27]) {
		glutDestroyWindow(win);
		destroyUniverse(uni);
		exit (0);
	}
}

void display (void) {

    keyOperations();
    
    stepWorld(uni, 1./60., 8);

    glClearColor(.20f, .4f, .8f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

	for (int i=0; i<15; i++) {

		glLoadIdentity(); 
		glTranslatef(0,-5,0);
		glRotatef(30,1,0,0); 
		
		bodyGetOpenGLMatrix(fallingBodies[i], &mat[0]);
		glMultMatrixf((GLfloat*)mat);
		
		int s = bodyGetShapeType(fallingBodies[i]);
		if (s==T_BOX) {
			glColor3f(1,1,1);
			glutSolidCube(1.0f);
			glColor3f(0,0,0);
			glutWireCube(1.0f);			
		}
		if (s==T_SPHERE) {
			glColor3f(1,1,1);
			glutSolidSphere(1.0f,16,8);
			glColor3f(0,0,0);
			glutWireSphere(1.0f,16,8);			
		}

	}

    glFlush();
    glutSwapBuffers();
}

void reshape (int width, int height) {
    glViewport(0, 0, (GLsizei)width, (GLsizei)height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60, (GLfloat)width / (GLfloat)height, 1.0, 100.0);
    glMatrixMode(GL_MODELVIEW);

}

void keyPressed (unsigned char key, int x, int y) {
	//printf("%i\n",key);
    keyStates[key] = 1;
}

void keyUp (unsigned char key, int x, int y) {
    keyStates[key] = 0;
}

int main (int argc, char **argv) {
	
	uni = createUniverse();
	setGravity(uni, 0,-9.98,0);

	void* groundShape = createBoxShape(uni, 100, 5, 100);	// size 100,100,5
	void* groundBody = createBody(uni, groundShape, 0, 0, -5, 0);	// 0 mass == static pos 0,0,-5
	bodySetRestitution(groundBody, .9);
	
	void* fallingShape = createBoxShape(uni, .5, .5, .5);
	fallingBodies[1] = createBody(uni, fallingShape, 10, 0, 12, -7);
	
	void* fallingShape2 = createSphereShape(uni, 1.);
	fallingBodies[0] = createBody(uni, fallingShape2, 10, 0.2, 6, -6.75);
	
	Vec t;
	t.x=20;t.y=20;t.z=20;
	bodyApplyTorque(fallingBodies[0], &t);
	bodySetRestitution(fallingBodies[0], .8);
	bodySetRestitution(fallingBodies[1], .8);
	


	
	for (int i=2; i<15; i++) {
		void* fs = createBoxShape(uni, 0.5, .5,.5);
		fallingBodies[i] = createBody(uni, fs, 1,  -2,(i-2)*1.1, -9);
		bodySetRotation(fallingBodies[i], .7,0,0);
		bodySetFriction(fallingBodies[i], .8);
	}
	

		
    glutInit(&argc, argv);
    glutInitDisplayMode (GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize (320, 240);
    glutInitWindowPosition (100, 100);
    win = glutCreateWindow ("yuk");

    glutDisplayFunc(display);
	glutIdleFunc(display);

    glutReshapeFunc(reshape);

    glutKeyboardFunc(keyPressed);
    glutKeyboardUpFunc(keyUp);

    glutMainLoop();
}
