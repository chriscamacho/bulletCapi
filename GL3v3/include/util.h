#include "gl_core_3_3.h"

// cast void* to a small ranged integer
#define asTiny(x) (long) x & 0xffff


enum shaderLocationType { shaderAttrib, shaderUniform };

GLuint create_shader(const char *filename, GLenum type);
GLuint getShaderLocation(int type, GLuint prog, const char *name);
char *file_read(const char *filename);

void glCheckError(char* f, int l);
int loadPNG(const char *filename);

