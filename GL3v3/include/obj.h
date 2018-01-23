#ifndef OBJ_H
#define OBJ_H

#include <GL/gl.h>
#include <kazmath.h>
#include <capi.h>

typedef struct __obj_t obj_t;

struct __obj_t {
	GLuint vao;
    GLuint vbo_vert, vbo_tex, vbo_norm;
    GLint vert_attrib, tex_attrib, norm_attrib;
    GLint sz_uniform, mvp_uniform, mv_uniform, tex_uniform;
    GLint lightDir_uniform, viewDir_uniform;
    int num_verts;
    GLuint program;
};


int createObj(obj_t *obj, int numVerts, float verts[], float txVert[],
              float norms[], char *vertShader, char *fragShader);
int createObjCopyShader(obj_t *obj, int numVerts, float verts[],
                        float txVert[], float norms[], obj_t *sdrobj);
void drawObj(obj_t *obj, Vec sz, int texu, kmMat4 * combined, kmMat4 * mv, kmVec3 lightDir, kmVec3 viewDir);

int loadObj(obj_t *obj,const char *objFile, char *vert, char *frag);
int loadObjCopyShader(obj_t *obj,const char *objFile, obj_t *sdrobj);

#endif
