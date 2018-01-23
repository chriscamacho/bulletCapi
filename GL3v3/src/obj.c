#include "gl_core_3_3.h"
#include <GL/gl.h>
#include <kazmath.h>
#include <stdio.h>
#include <stdlib.h>
#include "obj.h"
#include <stdbool.h>
#include "util.h"

// custom binary format not wavefront obj !
// see makeGBO.sh

/* each OBJ shader is expected to have at least these attribs and uniforms

vertex_attrib		vertex coordinates
uv_attrib			uv coordinates
mvp_uniform			combined model, view and projection matrix
mv_uniform			view, model matrix for lighting
texture_uniform		texture sampler

other custom attribs/uniforms should be handled as a special case in the
main code

*/

// TODO TODO TODO - release obj free all resources buffers etc


int loadObj(obj_t *obj,const char *objFile, char *vert, char *frag)
{
    FILE *pFile;
    pFile = fopen( objFile, "rb" );
    if (pFile==NULL) {
        printf("Cant find open model - %s\n",objFile);
        return false;
    }
    unsigned int magic;
    int NumVerts;

    fread (&magic,1, sizeof(unsigned int), pFile );
    if (magic!=0x614f4247) {
        printf("Does not appear to be a version 'a' GBO file\n");
        return false;
    }
    fread(&NumVerts,1,sizeof(unsigned int), pFile );

    float* Verts = malloc(sizeof(float) * 3 * NumVerts);
    fread(Verts,1,sizeof(float) * 3 * NumVerts, pFile );

    float* Norms = malloc(sizeof(float) * 3 * NumVerts);
    fread(Norms,1,sizeof(float) * 3 * NumVerts, pFile );

    float* TexCoords = malloc(sizeof(float) * 2 * NumVerts);
    fread(TexCoords,1,sizeof(float) * 2 * NumVerts, pFile );

    createObj(obj,NumVerts,Verts,TexCoords,Norms,vert,frag);

    free(TexCoords);
    free(Norms);
    free(Verts);

    return true;
}

int loadObjCopyShader(obj_t *obj,const char *objFile, obj_t *sdrobj)
{
    FILE *pFile;
    pFile = fopen( objFile, "rb" );
    if (pFile==NULL)
    {
        printf("Cant find open model - %s\n",objFile);
        return false;
    }
    unsigned int magic;
    int NumVerts;

    fread (&magic,1, sizeof(unsigned int), pFile );
    if (magic!=0x614f4247)
    {
        printf("Does not appear to be a version 'a' GBO file\n");
        return false;
    }
    fread(&NumVerts,1,sizeof(unsigned int), pFile );

    float* Verts = malloc(sizeof(float) * 3 * NumVerts);
    fread(Verts,1,sizeof(float) * 3 * NumVerts, pFile );

    float* Norms = malloc(sizeof(float) * 3 * NumVerts);
    fread(Norms,1,sizeof(float) * 3 * NumVerts, pFile );

    float* TexCoords = malloc(sizeof(float) * 2 * NumVerts);
    fread(TexCoords,1,sizeof(float) * 2 * NumVerts, pFile );

    createObjCopyShader(obj,NumVerts, Verts,TexCoords,
                        Norms, sdrobj);

    free(TexCoords);
    free(Norms);
    free(Verts);

    return true;
}


int createObj(obj_t *obj, int numVerts, float *verts, float *txVert,
              float *norms, char *vertShader, char *fragShader)
{
    obj->num_verts = numVerts;

    glGenVertexArrays(1, &obj->vao);
    glBindVertexArray(obj->vao);

    glGenBuffers(1, &obj->vbo_vert);
    glBindBuffer(GL_ARRAY_BUFFER, obj->vbo_vert);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * numVerts, verts,
                 GL_STATIC_DRAW);

    glGenBuffers(1, &obj->vbo_tex);
    glBindBuffer(GL_ARRAY_BUFFER, obj->vbo_tex);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 2 * numVerts, txVert,
                 GL_STATIC_DRAW);

    glGenBuffers(1, &obj->vbo_norm);
    glBindBuffer(GL_ARRAY_BUFFER, obj->vbo_norm);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * numVerts, norms,
                 GL_STATIC_DRAW);

    GLint link_ok = GL_FALSE;

    GLuint vs, fs;
    if ((vs = create_shader(vertShader, GL_VERTEX_SHADER)) == 0)
        return -1;
    if ((fs = create_shader(fragShader, GL_FRAGMENT_SHADER)) == 0)
        return -1;

    obj->program = glCreateProgram();
    glAttachShader(obj->program, vs);
    glAttachShader(obj->program, fs);
    glLinkProgram(obj->program);
    glGetProgramiv(obj->program, GL_LINK_STATUS, &link_ok);
    if (!link_ok)
    {
        fprintf(stderr,"error glLinkProgram\n");
        int len;
        GLchar log[1024];
        glGetProgramInfoLog(obj->program, 1024, &len, log);
        fprintf(stderr,"%s\n\n",log);
        return -1;
    }

    obj->vert_attrib =
        getShaderLocation(shaderAttrib, obj->program, "vertex_attrib");
    glCheckError(__FILE__,__LINE__);
    obj->tex_attrib =
        getShaderLocation(shaderAttrib, obj->program, "uv_attrib");
    glCheckError(__FILE__,__LINE__);
    obj->norm_attrib =
        getShaderLocation(shaderAttrib, obj->program, "norm_attrib");
    glCheckError(__FILE__,__LINE__);
    obj->mvp_uniform =
        getShaderLocation(shaderUniform, obj->program, "mvp_uniform");
    glCheckError(__FILE__,__LINE__);
    obj->mv_uniform =
        getShaderLocation(shaderUniform, obj->program, "mv_uniform");
    glCheckError(__FILE__,__LINE__);
    obj->tex_uniform =
        getShaderLocation(shaderUniform, obj->program, "u_texture");
    glCheckError(__FILE__,__LINE__);
    obj->lightDir_uniform =
        getShaderLocation(shaderUniform, obj->program, "u_lightDir");
    glCheckError(__FILE__,__LINE__);
    obj->viewDir_uniform =
        getShaderLocation(shaderUniform, obj->program, "u_viewDir");
    glCheckError(__FILE__,__LINE__);

    glEnableVertexAttribArray(obj->vert_attrib);
    glBindBuffer(GL_ARRAY_BUFFER, obj->vbo_vert);
    glVertexAttribPointer(obj->vert_attrib, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glEnableVertexAttribArray(obj->norm_attrib);
    glBindBuffer(GL_ARRAY_BUFFER, obj->vbo_norm);
    glVertexAttribPointer(obj->norm_attrib, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glEnableVertexAttribArray(obj->tex_attrib);
    glBindBuffer(GL_ARRAY_BUFFER, obj->vbo_tex);
    glVertexAttribPointer(obj->tex_attrib, 2, GL_FLOAT, GL_FALSE, 0, 0);
    
    glBindVertexArray(0);
    glCheckError(__FILE__,__LINE__);
    
    return 0;
}

/*
 *  create an obj from supplied verts using an existing models shader
 */
int createObjCopyShader(obj_t *obj, int numVerts, float *verts,
                        float *txVert, float *norms, obj_t *sdrobj)
{
    obj->num_verts = numVerts;

    glGenVertexArrays(1, &obj->vao);
    glBindVertexArray(obj->vao);

    glGenBuffers(1, &obj->vbo_vert);
    glBindBuffer(GL_ARRAY_BUFFER, obj->vbo_vert);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * numVerts, verts,
                 GL_STATIC_DRAW);


    glGenBuffers(1, &obj->vbo_tex);
    glBindBuffer(GL_ARRAY_BUFFER, obj->vbo_tex);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 2 * numVerts, txVert,
                 GL_STATIC_DRAW);


    glGenBuffers(1, &obj->vbo_norm);
    glBindBuffer(GL_ARRAY_BUFFER, obj->vbo_norm);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * numVerts, norms,
                 GL_STATIC_DRAW);

    obj->vert_attrib = sdrobj->vert_attrib;
    obj->tex_attrib = sdrobj->tex_attrib;
    obj->norm_attrib = sdrobj->norm_attrib;
    obj->mvp_uniform = sdrobj->mvp_uniform;
    obj->mv_uniform = sdrobj->mv_uniform;
    obj->tex_uniform = sdrobj->tex_uniform;
    obj->lightDir_uniform = sdrobj->lightDir_uniform;
    obj->viewDir_uniform =  sdrobj->viewDir_uniform;
    obj->program = sdrobj->program;

    glEnableVertexAttribArray(obj->vert_attrib);
    glBindBuffer(GL_ARRAY_BUFFER, obj->vbo_vert);
    glVertexAttribPointer(obj->vert_attrib, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glEnableVertexAttribArray(obj->norm_attrib);
    glBindBuffer(GL_ARRAY_BUFFER, obj->vbo_norm);
    glVertexAttribPointer(obj->norm_attrib, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glEnableVertexAttribArray(obj->tex_attrib);
    glBindBuffer(GL_ARRAY_BUFFER, obj->vbo_tex);
    glVertexAttribPointer(obj->tex_attrib, 2, GL_FLOAT, GL_FALSE, 0, 0);
    
    glBindVertexArray(0);
    glCheckError(__FILE__,__LINE__);
    
    return 0;
}

void drawObj(obj_t *obj, int texu, kmMat4 * combined, kmMat4 * mv, kmVec3 lightDir, kmVec3 viewDir)
{
    glUseProgram(obj->program);

    glBindVertexArray(obj->vao);

    glUniformMatrix4fv(obj->mvp_uniform, 1, GL_FALSE, (GLfloat *) combined);
    glUniformMatrix4fv(obj->mv_uniform, 1, GL_FALSE, (GLfloat *) mv);
	glUniform1i(obj->tex_uniform, texu);
    glUniform3f(obj->viewDir_uniform,viewDir.x,viewDir.y,viewDir.z);
    glUniform3f(obj->lightDir_uniform,lightDir.x,lightDir.y,lightDir.z);
    
    glDrawArrays(GL_TRIANGLES, 0, obj->num_verts);
    
    glBindVertexArray(0);

}
