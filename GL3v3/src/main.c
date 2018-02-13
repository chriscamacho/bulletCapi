
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

    void* groundShape = createBoxShape(uni, 20, 5, 20);	// size 20, 5, 20
    void* groundBody = createBody(uni, groundShape, 0, 0, -5, 0);	// 0 mass == static pos 0,0,-5
    bodySetRestitution(groundBody, 1.f);
	bodySetFriction(groundBody, 1.f);
    #define SLOPE 8.f
    bodySetRotationEular(groundBody, 0.01745329252f * SLOPE, 0, 0.01745329252f * SLOPE);

    for (int i=0; i<NumObj; i++) {
        void* fs;
        float sx,sy,sz;
        
        sx = 0.5f+rnd(1.0f);
        sy = 0.5f+rnd(1.0f);
        sz = 0.5f+rnd(1.0f);
        
        // 60, 120, 180, 240
        if (i<60) {
			// for lazyness all compounds have exactly 3 shapes
			fs = createCompoundShape(uni);
			void* c = createSphereShape(uni, 0.5f);
			addCompoundChild(fs, c, -1.f, 0, 0, 0,0,0);
			c = createSphereShape(uni, 0.5f);
			addCompoundChild(fs, c, 1.f, 0, 0, 0,0,0);
			c = createBoxShape(uni, 1.f, 0.25f, 0.25f);
			addCompoundChild(fs, c, 0,0,0, 0,0,0);
		} else {
			if (i<120) {
				fs = createBoxShape(uni, sx, sy, sz);
			} else {
				if (i<180) {
					sy=sx;
					sz=sx;
					fs = createSphereShape(uni, sx);
				} else {
					sx/=2.0f;
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
        phys[i].obj = createBody(uni, fs, (.5f+sx)*(.5f+sy)*(.5f+sz),  px, py, pz);
        
        bodySetRotationEular(phys[i].obj, rnd(6.28318530718), rnd(6.28318530718), rnd(6.28318530718));
        bodySetFriction(phys[i].obj, .4);
        bodySetRestitution(phys[i].obj, .6f);
    }

    float a=0;
    glCheckError(__FILE__,__LINE__);
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

        a+=0.01f;
        kmMat4 mod,mvp,mv;

        lightDir.x = cos(a*3.f)/2.f;
        lightDir.y = 1.f;
        lightDir.z = sin(a*3.f)/2.f;

        kmVec3Normalize(&lightDir, &lightDir);

        for (int i=0; i<NumObj; i++) {
			
			Vec p;
			bodyGetPosition(phys[i].obj, &p);
			if (p.y<-10) {
				p.x=0;
				p.y=30;
				p.z=0;
				bodySetPosition(phys[i].obj, p);
				p.x=0;
				p.y=0;
				p.z=0;
				bodySetLinearVelocity(phys[i].obj, p);
				bodySetAngularVelocity(phys[i].obj, p);
				p.x = rnd(6.28318530718);
				p.y = rnd(6.28318530718);
				p.z = rnd(6.28318530718);
				bodySetRotation(phys[i].obj, p);
			}
			
			
			bodyGetOpenGLMatrix(phys[i].obj, (float*)&mod);

            kmMat4Multiply(&mvp, &vp, &mod);
            kmMat4Multiply(&mv, &view, &mod);

            int s = bodyGetShapeType(phys[i].obj);

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
				Vec sz = {1.f,.25f,.25f,0.f};
				drawObj(&boxObj, sz , 1, &mvp, &mv, lightDir, viewDir);
				kmMat4 t;
				kmMat4Translation(&t, -1.f,0,0);
				kmMat4Multiply(&mod, &mod, &t);
				kmMat4Multiply(&mvp, &vp, &mod);
				kmMat4Multiply(&mv, &view, &mod);
				sz.x = .5f; sz.y = .5f; sz.z = .5f;
				drawObj(&ballObj, sz, 0, &mvp, &mv, lightDir, viewDir);
				kmMat4Translation(&t, 2.f,0,0);
				kmMat4Multiply(&mod, &mod, &t);
				kmMat4Multiply(&mvp, &vp, &mod);
				kmMat4Multiply(&mv, &view, &mod);
				drawObj(&ballObj, sz, 0, &mvp, &mv, lightDir, viewDir);			
			}
        }
        
        // for "static" ground shape
        Vec ss = { 20.f, 5.f, 20.f, 0.f };
        kmMat4Translation(&mod, 0.f, -5.f, 0.f);
        kmMat4 r;
        kmMat4RotationYawPitchRoll(&r, 0.01745329252f * SLOPE, 0, 0.01745329252f * SLOPE);
		kmMat4Multiply(&mod, &r, &mod);
		kmMat4Multiply(&mvp, &vp, &mod);
		kmMat4Multiply(&mv, &view, &mod);      
		drawObj(&boxObj, ss, 1, &mvp, &mv, lightDir, viewDir);

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


