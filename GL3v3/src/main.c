
#include "gl_core_3_3.h"
#include <GLFW/glfw3.h>

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "util.h"
#include "obj.h"

#include <kazmath.h>
#include <capi.h>
#include "phys.h"

void* uni;

#define NumObj 240

phys_t phys[NumObj];

kmVec3 pEye,pCentre,pUp,viewDir;
kmVec3 lightDir;
kmMat4 view,projection,vp;

obj_t ballObj, boxObj, drumObj;

#define Iwidth 960	// initial window size
#define Iheight 540

// the window size, view port (corner position & size)
float width = Iwidth;
float height = Iheight;
kmVec4 viewPort= {0,0,Iwidth,Iheight};

#define rnd(x) (float)rand()/(float)(RAND_MAX/x)

static void error_callback(int error, const char* description) {
    fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow* window, int key, int scancode,
                         int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

void framebufferSizeCallback(GLFWwindow* window, int w, int h)
{
	// this will cause some flickering during resize
	// however it is much quicker that resizing each
	// frame...
	glScissor(0,0,width,height);
	glClearColor(0.f,0.f,0.f,1.f);
	glClear(GL_COLOR_BUFFER_BIT);
	glfwSwapBuffers(window);
	glClear(GL_COLOR_BUFFER_BIT);
	
    width = w;
    height = h;

    GLfloat aspect = 16.f / 9.f;

    viewPort.z = width;
    viewPort.w = (float)width / aspect;

    if (viewPort.w > height)
    {
        viewPort.w = height;
        viewPort.z = height * aspect;
    }

    viewPort.x = (width - viewPort.z) / 2;
    viewPort.y = (height - viewPort.w) / 2;
}


void windowSizeCallback(GLFWwindow* window, int w, int h) {


}

int main(void) {

    GLFWwindow* window;

    int seed = time(NULL);
    srand(seed);

    glfwSetErrorCallback(error_callback);

    if (!glfwInit())
        exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);

    window = glfwCreateWindow(width, height, "Simple example", NULL, NULL);
    if (!window) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwSetKeyCallback(window, key_callback);
//    glfwSetWindowSizeCallback(window, windowSizeCallback);
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

    glfwMakeContextCurrent(window);

    if(ogl_LoadFunctions() == ogl_LOAD_FAILED) {
        fprintf(stderr,"failed to load GL function pointers");
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwSwapInterval(1);

    int mtu;
    glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &mtu);
    printf("max tx units %i\n",mtu);


    glActiveTexture(GL_TEXTURE0);
    GLuint ballTex = loadPNG("data/ball.png");
    glBindTexture(GL_TEXTURE_2D, ballTex);
    glActiveTexture(GL_TEXTURE1);
    GLuint boxTex = loadPNG("data/box.png");
    glBindTexture(GL_TEXTURE_2D, boxTex);
    glActiveTexture(GL_TEXTURE2);
    GLuint drumTex = loadPNG("data/barrel.png");
    glBindTexture(GL_TEXTURE_2D, drumTex);
    glActiveTexture(GL_TEXTURE3);
    GLuint sunTex = loadPNG("data/sun.png");
    glBindTexture(GL_TEXTURE_2D, sunTex);

    loadObj(&boxObj, "data/box.gbo",
            "data/textured.vert", "data/textured.frag");
    loadObjCopyShader(&ballObj,"data/ball.gbo", &boxObj);
    loadObjCopyShader(&drumObj,"data/drum.gbo", &boxObj);


    kmMat4Identity(&view);

    pEye.x = 0;
    pEye.y = 12;
    pEye.z = 50;
    pCentre.x = 0;
    pCentre.y = 0;
    pCentre.z = 0;
    pUp.x = 0;
    pUp.y = 1;
    pUp.z = 0;

    kmMat4LookAt(&view, &pEye, &pCentre, &pUp);
    kmVec3Subtract(&viewDir,&pEye,&pCentre);
    kmVec3Normalize(&viewDir,&viewDir);

    kmMat4Identity(&projection);
    kmMat4PerspectiveProjection(&projection, 45,
                                (float)width / (float)height, 0.1, 1000);

	kmMat4Multiply(&vp, &projection, &view);

    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_SCISSOR_TEST);

    uni = createUniverse();
    setGravity(uni, 0,-9.98,0);

	{
		Vec sz = {20, 5, 20};
		void* groundShape = createBoxShape(uni, sz.x, sz.y, sz.z);	// size 20, 5, 20
		phys[0].body = createBody(uni, groundShape, 0, 0, -5, 0);	// 0 mass == static pos 0,0,-5
		#define SLOPE 4.f

		bodySetRestitution(phys[0].body, 1.f);
		bodySetFriction(phys[0].body, 1.f);
		bodySetRotationEular(phys[0].body, 0.01745329252f * SLOPE, 0, 0.01745329252f * SLOPE);

		phys[0].sz = sz;
	}
	
	void* hinge;
	{
		Vec sz = {2, .5, 4};
		void* shp = createBoxShape(uni, sz.x, sz.y, sz.z);
		phys[1].body = createBody(uni, shp, 0,  2.2, 8, 0);
		phys[1].sz = sz;
		
		sz = (Vec){2, 8., .5};
		shp = createBoxShape(uni, sz.x, sz.y, sz.z);
		phys[2].body = createBody(uni, shp, 20,  -2.2, 8, 0);
		phys[2].sz = sz;
		bodySetDeactivation(phys[2].body, false);
	
		
		Vec pivA, rotA, pivB, rotB;
		
		pivA = (Vec){-2.3,0,0};
		rotA = (Vec){0,1.5707963268,0};
		pivB = (Vec){2.3,0,0};
		rotB = (Vec){0,1.5707963268,0};
		
		hinge = createHinge(uni, phys[1].body, phys[2].body, pivA, rotA, 
									pivB, rotB, false, false);
		hingeSetLimit(hinge, -PI / 2.0f , PI / 2.0f); // > pi == unlimited
		hingeEnableAngularMotor(hinge, true, 
						-2, 10000);


		setConstraintParam(hinge, C_ERP, 1, -1);
		setConstraintParam(hinge, C_CFM, 0, -1);

		
	}
	
	
    for (int i=3; i<NumObj; i++) {
        void* fs;
        float sx,sy,sz;
        
        sx = 0.5f+rnd(1.0f);
        sy = 0.5f+rnd(1.0f);
        sz = 0.5f+rnd(1.0f);
        
        // 60, 120, 180, 240
        if (i<30) {
			// for lazyness all compounds have exactly 3 shapes
			fs = createCompoundShape(uni);
			void* c = createSphereShape(uni, 1.f);
			addCompoundChild(fs, c, -1.5f, 0, 0, 0,0,0);
			c = createSphereShape(uni, 1.f);
			addCompoundChild(fs, c, 1.5f, 0, 0, 0,0,0);
			// box made long on wrong axis to check local
			// orientation changes of shape on the compound works
			c = createBoxShape(uni, 3.f, 0.5f, 0.5f);
			addCompoundChild(fs, c, 0,1.f,0, 0.01745329252f * 90,0,0);
		} else {
			if (i<105) {
				fs = createBoxShape(uni, sx, sy, sz);
			} else {
				if (i<150) {
					sy=sx;
					sz=sx;
					fs = createSphereShape(uni, sx);
				} else {
					sx /= 2.0f;
					sy = sx;
					sz *= 2.0f;
					fs = createCylinderShape(uni, sx, sz);
				}
			}
		}
        phys[i].sz.x = sx;
        phys[i].sz.y = sy;
        phys[i].sz.z = sz;
        float px = 10.f-rnd(20.f);
        float py = 30.f-rnd(8.f);
        float pz = 10.f-rnd(20.f);
        phys[i].body = createBody(uni, fs, (.5f+sx)*(.5f+sy)*(.5f+sz),  px, py, pz);
        
        bodySetRotationEular(phys[i].body, rnd(6.28318530718), rnd(6.28318530718), rnd(6.28318530718));
        bodySetFriction(phys[i].body, .4);
        bodySetRestitution(phys[i].body, .6f);
    }

    float a=0;
    glCheckError(__FILE__,__LINE__);
    
    int hingeCount = 0;
    int hingeDir = -2;
    
    while (!glfwWindowShouldClose(window)) {
        // draw the whole window black
/*
        glScissor(0,0,width,height);
        glClearColor(0.f,0.f,0.f,1.f);
        glClear(GL_COLOR_BUFFER_BIT);
*/
        // then clear the area representing 16:9 area
        glViewport(viewPort.x, viewPort.y, viewPort.z, viewPort.w);
        glScissor(viewPort.x, viewPort.y, viewPort.z, viewPort.w);

        glClearColor(0.25f,0.5f,1.f,1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        stepWorld(uni, 1.f/60.f, 8);
        
        hingeCount++;
        if (hingeCount > 240) {
			hingeCount = 0;
			hingeDir = -hingeDir;
			hingeEnableAngularMotor(hinge, true, 
						hingeDir, 10000);
		}

        a+=0.01f;
        kmMat4 mod,mvp,mv;

        lightDir.x = cos(a*3.f)/2.f;
        lightDir.y = 1.f;
        lightDir.z = sin(a*3.f)/2.f;

        kmVec3Normalize(&lightDir, &lightDir);

        for (int i=0; i<NumObj; i++) {
			
			Vec p;
			bodyGetPosition(phys[i].body, &p);
			if (p.y<-10 && i>2) {
				p.x=0;
				p.y=30;
				p.z=0;
				bodySetPosition(phys[i].body, p);
				p.x=0;
				p.y=0;
				p.z=0;
				bodySetLinearVelocity(phys[i].body, p);
				bodySetAngularVelocity(phys[i].body, p);
				p.x = rnd(6.28318530718);
				p.y = rnd(6.28318530718);
				p.z = rnd(6.28318530718);
				bodySetRotation(phys[i].body, p);
			}
			
			
			bodyGetOpenGLMatrix(phys[i].body, (float*)&mod);

            kmMat4Multiply(&mvp, &vp, &mod);
            kmMat4Multiply(&mv, &view, &mod);

            int s = bodyGetShapeType(phys[i].body);

            if (s==T_BOX) {
                drawObj(&boxObj, phys[i].sz, 1,&mvp, &mv, lightDir, viewDir);
            }
            if (s==T_SPHERE) {
                drawObj(&ballObj, phys[i].sz, 0,&mvp, &mv, lightDir, viewDir);
            }
            if (s==T_CYLINDER) {
                drawObj(&drumObj, phys[i].sz, 2,&mvp, &mv, lightDir, viewDir);
			}
			if (s==T_COMPOUND) { // for lazyness all compounds the same

				Vec sz = { 1.f, 1.f, 1.f };
				kmMat4 t,t2;
				
				kmMat4Translation(&t, -1.5f,0,0);
				kmMat4Multiply(&mod, &mod, &t);		
				kmMat4Multiply(&mvp, &vp, &mod);
				kmMat4Multiply(&mv, &view, &mod);
				
				drawObj(&ballObj, sz, 3, &mvp, &mv, lightDir, viewDir);
				
				kmMat4Translation(&t, 3.f,0,0);
				kmMat4Multiply(&mod, &mod, &t);
				kmMat4Multiply(&mvp, &vp, &mod);
				kmMat4Multiply(&mv, &view, &mod);
				drawObj(&ballObj, sz, 3, &mvp, &mv, lightDir, viewDir);	
				
				sz = (Vec){3.f,.5f,.5f,0.f};
				kmMat4Translation(&t, -1.5f,1.f,0);
				kmMat4RotationY(&t2, 0.01745329252f * 90.f);
				kmMat4Multiply(&mod, &mod, &t);						
				kmMat4Multiply(&mod, &mod, &t2);		

				kmMat4Multiply(&mvp, &vp, &mod);
				kmMat4Multiply(&mv, &view, &mod);

				drawObj(&boxObj, sz , 3, &mvp, &mv, lightDir, viewDir);
			}
        }

        glfwSwapBuffers(window);
        glfwPollEvents();

        // for testing
        //glfwSetWindowShouldClose(window, GLFW_TRUE);
    }

    // just to catch anything missed...
    glCheckError(__FILE__,__LINE__);

	destroyUniverse(uni);

    glfwDestroyWindow(window);

    glfwTerminate();
    exit(EXIT_SUCCESS);
}


