
#include "gl_core_3_3.h"
#include <GLFW/glfw3.h>

#include <stdlib.h>
#include <stdio.h>

#include "util.h"
#include "obj.h"

#include <kazmath.h>
#include <capi.h>

void* uni;
void* fallingBodies[30];

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

static void error_callback(int error, const char* description) {
    fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow* window, int key, int scancode,
                         int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}


void windowSizeCallback(GLFWwindow* window, int w, int h) {

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

int main(void) {

    GLFWwindow* window;


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
    glfwSetWindowSizeCallback(window, windowSizeCallback);

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

    lightDir.x = 0.7;
    lightDir.y = 0.7;
    lightDir.z = 0.7;
    kmVec3Normalize(&lightDir, &lightDir);

    kmMat4Identity(&view);

    pEye.x = 0;
    pEye.y = 8;
    pEye.z = 12;
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

    kmMat4Assign(&vp, &projection);
    kmMat4Multiply(&vp, &vp, &view);

    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_SCISSOR_TEST);

    uni = createUniverse();
    setGravity(uni, 0,-9.98,0);

    void* groundShape = createBoxShape(uni, 100, 5, 100);	// size 100,100,5
    void* groundBody = createBody(uni, groundShape, 0, 0, -5, 0);	// 0 mass == static pos 0,0,-5
    bodySetRestitution(groundBody, .9);

    for (int i=0; i<30; i++) {
        void* fs;
        if (i<5) {
            fs = createBoxShape(uni, 0.5, 0.5, 0.5);
        } else {
			if (i<10) {
				fs = createSphereShape(uni, 0.5);
			} else {
				fs = createCylinderShape(uni, 0.5, 0.5);
			}
        }
        fallingBodies[i] = createBody(uni, fs, 1,  0,(i-2)*1.1, 0);
        bodySetRotation(fallingBodies[i], .7,0,0);
        bodySetFriction(fallingBodies[i], .8);
    }

    float a;
    glCheckError(__FILE__,__LINE__);
    while (!glfwWindowShouldClose(window)) {
        // draw the whole window black
        glScissor(0,0,width,height);
        glClearColor(0.f,0.f,0.f,1.f);
        glClear(GL_COLOR_BUFFER_BIT);

        // then clear the area representing 16:9 area
        glViewport(viewPort.x, viewPort.y, viewPort.z, viewPort.w);
        glScissor(viewPort.x, viewPort.y, viewPort.z, viewPort.w);
        glClearColor(0.25f,0.5f,1.f,1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        stepWorld(uni, 1./60., 8);

        a+=0.01;
        kmMat4 mod,tm,mvp,mv;

        lightDir.x = cos(-a*6);
        lightDir.y = sin(-a*6);
        lightDir.z = 1;

        kmVec3Normalize(&lightDir, &lightDir);
        /*
        //-----
                kmMat4Identity(&mod);
                kmMat4Translation(&mod, -1.5,0,0);
        //        kmMat4RotationYawPitchRoll(&tm, a, a*1.5, 0 );
                kmMat4RotationYawPitchRoll(&tm, 0,0, 0 );
                kmMat4Multiply(&mod,&mod,&tm);

                kmMat4Assign(&mvp, &vp);
                kmMat4Multiply(&mvp, &mvp, &mod);

                kmMat4Assign(&mv, &view);
                kmMat4Multiply(&mv, &mv, &mod);

                drawObj(&boxObj, 1, &mvp, &mv, lightDir, viewDir);
        //----
                kmMat4Identity(&mod);
                kmMat4Translation(&mod, 1.5,0,0);
        //        kmMat4RotationYawPitchRoll(&tm, a, a*1.5, 0 );
                kmMat4RotationYawPitchRoll(&tm, 0,0, 0 );
                kmMat4Multiply(&mod,&mod,&tm);

                kmMat4Assign(&mvp, &vp);
                kmMat4Multiply(&mvp, &mvp, &mod);

                kmMat4Assign(&mv, &view);
                kmMat4Multiply(&mv, &mv, &mod);


                drawObj(&ballObj, 0,&mvp, &mv, lightDir, viewDir);
        //-----
                kmMat4Identity(&mod);
                kmMat4Translation(&mod, 0,0,0);
        //        kmMat4RotationYawPitchRoll(&tm, a, a*1.5, 0 );
                kmMat4RotationYawPitchRoll(&tm, 0,0, 0 );
                kmMat4Multiply(&mod,&mod,&tm);

                kmMat4Assign(&mvp, &vp);
                kmMat4Multiply(&mvp, &mvp, &mod);

                kmMat4Assign(&mv, &view);
                kmMat4Multiply(&mv, &mv, &mod);


                drawObj(&drumObj, 2,&mvp, &mv, lightDir, viewDir);
        //-----
        */
        for (int i=0; i<30; i++) {
            bodyGetOpenGLMatrix(fallingBodies[i], (float*)&mod);

            kmMat4Assign(&mvp, &vp);
            kmMat4Multiply(&mvp, &mvp, &mod);

            kmMat4Assign(&mv, &view);
            kmMat4Multiply(&mv, &mv, &mod);
            int s = bodyGetShapeType(fallingBodies[i]);
            if (s==T_BOX) {
                drawObj(&boxObj, 0,&mvp, &mv, lightDir, viewDir);
            }
            if (s==T_SPHERE) {
                drawObj(&ballObj, 0,&mvp, &mv, lightDir, viewDir);
            }
            if (s==T_CYLINDER) {
                drawObj(&drumObj, 0,&mvp, &mv, lightDir, viewDir);
			}
        }

        glfwSwapBuffers(window);
        glfwPollEvents();

        // for testing
        //glfwSetWindowShouldClose(window, GLFW_TRUE);
    }

    // just to catch anything missed...
    glCheckError(__FILE__,__LINE__);


    glfwDestroyWindow(window);

    glfwTerminate();
    exit(EXIT_SUCCESS);
}


