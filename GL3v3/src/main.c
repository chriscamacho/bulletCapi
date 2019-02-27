
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

#define NumObj 480

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

void* vehicleBody;
void* wheelConstr[4];
void* wheelBody[4];

static void error_callback(int error, const char* description) {
    fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow* window, int key, int scancode,
                         int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

/*
 * 
 *  framebuffer resize keeps aspect ratio constant 
 * 
 */
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
    
    /*
     * 
     * set up OpenGL stuff
     * 
     * 
     */

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

    pEye.x = 40;
    pEye.y = 8;
    pEye.z = 20;
    pCentre.x = 10;
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
    
    /*
     * 
     * setup physics stuff 
     * 
     */
     
    uni = universeCreate();
    universeSetGravity(uni, 0,-9.98,0);

	{ // ground
		Vec sz = {200, 5, 200};
		void* groundShape = shapeCreateBox(uni, sz.x, sz.y, sz.z);	// size 20, 5, 20
		phys[0].body = bodyCreate(uni, groundShape, 0, 180, 20, -180);	// 0 mass == static pos 0,0,-5
		#define SLOPE 4.f

		bodySetRestitution(phys[0].body, 1.f);
		bodySetFriction(phys[0].body, 1.f);
		bodySetRotationEular(phys[0].body, 0.01745329252f * SLOPE, 0, 0.01745329252f * SLOPE);


		phys[0].sz = sz;
	}
	
	void* hinge;
	{ // paddle thingie to move other shapes about
    
		Vec sz = {2, .5, 4};
		void* shp = shapeCreateBox(uni, sz.x, sz.y, sz.z);
		phys[1].body = bodyCreate(uni, shp, 0,  -4.2, 8, 0);
		bodySetRotationEular(phys[1].body, 0.01745329252f * SLOPE, 0, 0.01745329252f * SLOPE);
		phys[1].sz = sz;
		
		sz = (Vec){8, 8, .5};
		shp = shapeCreateBox(uni, sz.x, sz.y, sz.z);
		phys[2].body = bodyCreate(uni, shp, 40,  0.2, 8, 0);
		phys[2].sz = sz;
		bodySetDeactivation(phys[2].body, false);
	
		
		Vec pivA, rotA, pivB, rotB;
		
		pivA = (Vec){2.3,0,0};
		rotA = (Vec){0,1.5707963268,0};
		pivB = (Vec){-8,0,0};
		rotB = (Vec){0,1.5707963268,0};
		
		hinge = hingeCreate(uni, phys[1].body, phys[2].body, pivA, rotA, 
									pivB, rotB, false, false);
		hingeSetLimit(hinge, -PI / 2.0f , PI / 2.0f); // > pi == unlimited
		hingeEnableAngularMotor(hinge, true, 
						-4, 100);
		constraintSetParam(hinge, C_ERP, 1, -1);
		constraintSetParam(hinge, C_CFM, 0, -1);

		
	}

    // just for clarity
    #define engineAxis 3
    #define steeringAxis 5
    #define suspensionAxis 2

	{   /*
         *
         *    set up vehicle
         * 
         */

        void* chass;
        void* vehicleShape;
        chass = shapeCreateBox(uni, 1.f, 0.5f, 2.f);
		vehicleShape = shapeCreateCompound(uni);
		compoundAddChild(vehicleShape, chass, 0, 1, 0, 0,0,0);
		//vehicleShape = shapeCreateBox(uni, 1.f, 0.5f, 2.f);
        #define fheight 4.f
		vehicleBody = bodyCreate(uni, vehicleShape, 200, 40, fheight-1, 0);
        

		Vec wheelPos[4] = {
			{40-1.5,fheight-1., 2},
			{40+1.5,fheight-1., 2},
			{40+1.5,fheight-1.,-2},
			{40-1.5,fheight-1.,-2}
		};

        bodySetDeactivation(vehicleBody, false);
        void* wheelShape = shapeCreateCylinderX(uni, 0.6, 0.4);

		for (int i = 0; i<4; i++) {

			wheelBody[i] = bodyCreate(uni, wheelShape, 10, 
                                            wheelPos[i].x, 
                                            wheelPos[i].y, 
                                            wheelPos[i].z);
            bodySetFriction(wheelBody[i],1000);
            bodySetDeactivation(wheelBody[i], false);
            
            Vec parentAxis = {0,1,0};
            Vec childAxis = {1,0,0};
            //Vec anchor = {20,fheight,0};
            Vec anchor = wheelPos[i];
            wheelConstr[i] = hinge2Create(uni, vehicleBody, wheelBody[i],
                                    anchor, parentAxis, childAxis, true);

            
            if (i<2) { // engine motor
                hinge2enableMotor(wheelConstr[i], engineAxis, true);
                hinge2setMaxMotorForce(wheelConstr[i], engineAxis, 200);
                hinge2setTargetVelocity(wheelConstr[i], engineAxis, 0);

                hinge2enableMotor(wheelConstr[i], steeringAxis, true);
                hinge2setMaxMotorForce(wheelConstr[i], steeringAxis, 200);
                hinge2setTargetVelocity(wheelConstr[i], steeringAxis, 0);
                hinge2setLowerLimit(wheelConstr[i], -.5);
                hinge2setUpperLimit(wheelConstr[i], .5);
            } else {
                hinge2setLowerLimit(wheelConstr[i], 0);
                hinge2setUpperLimit(wheelConstr[i], 0);
            }
            constraintSetParam(wheelConstr[i], C_CFM, .15f, suspensionAxis);//0.15f, 2);
            constraintSetParam(wheelConstr[i], C_ERP, .35f, suspensionAxis); //0.35f, 2);

            hinge2setDamping(wheelConstr[i],  suspensionAxis, 200.0, false);
            hinge2setStiffness(wheelConstr[i], suspensionAxis, 2000.0, false);

        }
	}
    
	/*
     * 
     * set up all the "loose" physics objects
     * 
     */
	
    for (int i=3; i<NumObj; i++) {
        void* fs;
        float sx,sy,sz;
        
        sx = 0.25f+rnd(.5f);
        sy = 0.25f+rnd(.5f);
        sz = 0.25f+rnd(.5f);
        
        // 60, 120, 180, 240
        if (i<30) {
			// for lazyness all compounds are identical having exactly shapes
			fs = shapeCreateCompound(uni);
			void* c = shapeCreateSphere(uni, .5f);
			compoundAddChild(fs, c, -.75f, 0, 0, 0,0,0);
			c = shapeCreateSphere(uni, 1.f);
			compoundAddChild(fs, c, .75f, 0, 0, 0,0,0);
			// box made long on wrong axis to check local
			// orientation changes of shape on the compound works
			c = shapeCreateBox(uni, 1.5f, 0.25f, 0.25f);
			compoundAddChild(fs, c, -.5,.5f,0, 0.01745329252f * 90,0,0);
		} else {
			if (i<105) {
				fs = shapeCreateBox(uni, sx, sy, sz);
			} else {
				if (i<150) {
					sy=sx;
					sz=sx;
					fs = shapeCreateSphere(uni, sx);
				} else {
					sx /= 2.0f;
					sy = sx;
					sz *= 2.0f;
					fs = shapeCreateCylinderZ(uni, sx, sz);
				}
			}
		}
        phys[i].sz.x = sx;
        phys[i].sz.y = sy;
        phys[i].sz.z = sz;
        float px = 20.f-rnd(40.f);
        float py = 30.f-rnd(8.f);
        float pz = 20.f-rnd(40.f);
        phys[i].body = bodyCreate(uni, fs, (.5f+sx)*(.5f+sy)*(.5f+sz),  px, py, pz);
        
        bodySetRotationEular(phys[i].body, rnd(6.28318530718), rnd(6.28318530718), rnd(6.28318530718));
        bodySetFriction(phys[i].body, .4);
        bodySetRestitution(phys[i].body, .6f);
    }

    float a=0;
    glCheckError(__FILE__,__LINE__);
    
    int hingeCount = 0;
    int hingeDir = -4;
    float steer=0;
    float engine=0;
    
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

        universeStep(uni, 1.f/60.f, 8);
        
        hingeCount++;
        if (hingeCount > 80) {
			hingeCount = 0;
			hingeDir = -hingeDir;
			hingeEnableAngularMotor(hinge, true, 
						hingeDir, 400);
		}

        a+=0.01f;
        kmMat4 mod,mvp,mv;

        lightDir.x = cos(a*3.f)/2.f;
        lightDir.y = 1.f;
        lightDir.z = sin(a*3.f)/2.f;

        kmVec3Normalize(&lightDir, &lightDir);

        for (int i=0; i<NumObj; i++) {
//        for (int i=0; i<3; i++) {
			
			Vec p;
			if (phys[i].body != 0) {
				bodyGetPosition(phys[i].body, &p);
				if (p.y<-10 && i>2) {
					
					p.x=4;
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
					
					// TODO (check further) compounds don't seem to leak ???
					// removing children before causes seg or double free...
					/*
					removeBody(uni, phys[i].body);					
					deleteShape(uni, bodyGetShape(phys[i].body));
					deleteBody(phys[i].body);
					phys[i].body = 0;
					*/
					
				}
			}
			
			if (phys[i].body != 0) {
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

					Vec sz = { .5,.5,.5 };
					kmMat4 t,t2;
					
					kmMat4Translation(&t, -.75f,0,0);
					kmMat4Multiply(&mod, &mod, &t);		
					kmMat4Multiply(&mvp, &vp, &mod);
					kmMat4Multiply(&mv, &view, &mod);
					
					drawObj(&ballObj, sz, 3, &mvp, &mv, lightDir, viewDir);
					
					kmMat4Translation(&t, .75f,0,0);
					kmMat4Multiply(&mod, &mod, &t);
					kmMat4Multiply(&mvp, &vp, &mod);
					kmMat4Multiply(&mv, &view, &mod);
					drawObj(&ballObj, sz, 3, &mvp, &mv, lightDir, viewDir);	
					
					sz = (Vec){1.5f,.25f,.25f,0.f};
					kmMat4Translation(&t, -.5,.5f,0);
					kmMat4RotationY(&t2, 0.01745329252f * 90.f);
					kmMat4Multiply(&mod, &mod, &t);						
					kmMat4Multiply(&mod, &mod, &t2);		

					kmMat4Multiply(&mvp, &vp, &mod);
					kmMat4Multiply(&mv, &view, &mod);

					drawObj(&boxObj, sz , 3, &mvp, &mv, lightDir, viewDir);
				} // end of create compound code
			}
        }
        
        
        /*
         * 
         * handle vehicle control
         * 
         * 
         */

        bool noSteer;
        noSteer = true;
        float a1 = hinge2getAngle1(wheelConstr[0]);
        float a2 = hinge2getAngle1(wheelConstr[1]);
        #define mt .5
        if (glfwGetKey(window, GLFW_KEY_LEFT) ) {
            noSteer = false;
            steer+=1;
        }
        if (glfwGetKey(window, GLFW_KEY_RIGHT) ) {
            noSteer = false;
            steer-=1;
        }
        
        // seems the two steered wheels can get out of sync so
        // auto centre them independantly
        if (noSteer) {
            #define ctr 0.01
            steer=0;
            if (a1>ctr) steer=1;
            if (a1<ctr) steer=-1;
            hinge2setTargetVelocity(wheelConstr[0], steeringAxis, -steer);
            steer=0;
            if (a2>ctr) steer=1;
            if (a2<ctr) steer=-1;
            hinge2setTargetVelocity(wheelConstr[1], steeringAxis, -steer);
        }
        
        if (glfwGetKey(window, GLFW_KEY_UP)) {
            engine+=1;
            if (engine>80) engine=80;
        } else {
            engine*=.99f;
        }
        
        if (!noSteer) {
            hinge2setTargetVelocity(wheelConstr[0], steeringAxis, -steer);
            hinge2setTargetVelocity(wheelConstr[1], steeringAxis, -steer);
        }
        hinge2setTargetVelocity(wheelConstr[0], engineAxis, -engine);
        hinge2setTargetVelocity(wheelConstr[1], engineAxis, -engine);
        
        //printf("angle %f, %f\n",hinge2getAngle1(wheelConstr[0]),hinge2getAngle2(wheelConstr[0]));
        
        /*
         * 
         *  draw vehicle here
         * 
         */
        bodyGetOpenGLMatrix(vehicleBody, (float*)&mod);
        
        kmMat4 t;	// 2 up to compensate for centre of gravity cheat
        kmMat4Translation(&t,0,1,0);
        kmMat4Multiply(&mod,&mod,&t);
        
        kmMat4Multiply(&mvp, &vp, &mod);
        kmMat4Multiply(&mv, &view, &mod);
        Vec sz = {1,.5,2};
        drawObj(&boxObj, sz, 1,&mvp, &mv, lightDir, viewDir);
        
        kmMat4 r;
        kmMat4RotationY(&r, 0.01745329252f * 90.f);
        for (int i=0; i<4; i++) {
            Vec sz={.6,.6,.4};
            bodyGetOpenGLMatrix(wheelBody[i], (float*)&mod);
            kmMat4Multiply(&mod, &mod, &r);
            kmMat4Multiply(&mvp, &vp, &mod);
            kmMat4Multiply(&mv, &view, &mod);
            drawObj(&drumObj, sz, 2,&mvp, &mv, lightDir, viewDir);
        }
        
        Vec p;
        bodyGetPosition(vehicleBody, &p);
        kmMat4LookAt(&view, &pEye, (kmVec3*)(&p), &pUp);
        kmVec3Subtract(&viewDir,&pEye,&pCentre);
        kmVec3Normalize(&viewDir,&viewDir);
        kmMat4Multiply(&vp, &projection, &view);
        
        
        
        glfwSwapBuffers(window);
        glfwPollEvents();

        // for testing
        //glfwSetWindowShouldClose(window, GLFW_TRUE);
    }

    // just to catch anything missed...
    glCheckError(__FILE__,__LINE__);

	universeDestroy(uni);

    glfwDestroyWindow(window);

    glfwTerminate();
    exit(EXIT_SUCCESS);
}


