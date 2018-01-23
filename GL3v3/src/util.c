#include <stdio.h>
#include "util.h"
#include "lodepng.h"
#include <stdlib.h>

float rand_range(float start,float range) {
    return start + range * ((float)rand() / RAND_MAX) ;
}


/**
 * Store all the file's contents in memory, useful to pass shaders
 * source code to OpenGL
 */
char *file_read(const char *filename)
{
    FILE *in = fopen(filename, "rb");
    if (in == NULL)
        return NULL;

    int res_size = BUFSIZ;
    char *res = (char *)malloc(res_size);
    int nb_read_total = 0;

    while (!feof(in) && !ferror(in))
    {
        if (nb_read_total + BUFSIZ > res_size)
        {
            if (res_size > 10 * 1024 * 1024)
                break;
            res_size = res_size * 2;
            res = (char *)realloc(res, res_size);
        }
        char *p_res = res + nb_read_total;
        nb_read_total += fread(p_res, 1, BUFSIZ, in);
    }

    fclose(in);
    res = (char *)realloc(res, nb_read_total + 1);
    res[nb_read_total] = '\0';
    return res;
}


GLuint getShaderLocation(int type, GLuint prog, const char *name)
{
    GLuint ret=-1;
    if (type == shaderAttrib)
        ret = glGetAttribLocation(prog, name);
    if (type == shaderUniform)
        ret = glGetUniformLocation(prog, name);
    if (ret == -1)
    {
        printf("Cound not bind shader location %s\n", name);
    }
    return ret;
}

/**
 * Compile the shader from file 'filename', with error handling
 */
GLuint create_shader(const char *filename, GLenum type) {
    const GLchar *source = file_read(filename);
    if (source == NULL)
    {
        fprintf(stderr, "Error opening %s: ", filename);
        perror("");
        return 0;
    }
    GLuint res = glCreateShader(type);
    const GLchar *sources[] = { source };
    glShaderSource(res, 1, sources, NULL);
    free((void *)source);

    glCompileShader(res);
    GLint compile_ok = GL_FALSE;
    glGetShaderiv(res, GL_COMPILE_STATUS, &compile_ok);
    if (compile_ok == GL_FALSE)
    {
        fprintf(stderr, "%s:\n", filename);
        int len;
        GLchar log[1024];
        glGetShaderInfoLog(res, 1024, &len, log);
        fprintf(stderr,"%s\n\n",log);
        glDeleteShader(res);
        return 0;
    }

    return res;
}

struct token_string
{
    GLuint Token;
    const char *String;
};

static const struct token_string Errors[] = {
    { GL_NO_ERROR, "no error" },
    { GL_INVALID_ENUM, "invalid enumerant" },
    { GL_INVALID_VALUE, "invalid value" },
    { GL_INVALID_OPERATION, "invalid operation" },
    { GL_OUT_OF_MEMORY, "out of memory" },
#ifdef GL_EXT_framebuffer_object
    { GL_INVALID_FRAMEBUFFER_OPERATION_EXT, "invalid framebuffer operation" },
#endif
    { ~0, NULL } /* end of list indicator */
};



const GLubyte*
glErrorString(GLenum errorCode)
{
    int i;
    for (i = 0; Errors[i].String; i++) {
        if (Errors[i].Token == errorCode)
            return (const GLubyte *) Errors[i].String;
    }

    return (const GLubyte *) 0;
}

void glCheckError(char* f, int l) {
    int e = glGetError();
    if (e == GL_NO_ERROR) return;
    fprintf(stderr,"file %s line %i: %s\n",f,l,glErrorString(e));
}

// returns GL texture handle
int loadPNG(const char *filename) {

    unsigned error;
    unsigned char* image;
    unsigned w, h;
    GLuint texture;
    LodePNGState state;
    unsigned char* png;
    size_t pngsize;
    size_t components;

    lodepng_state_init(&state);
    lodepng_load_file(&png, &pngsize, filename);
    error = lodepng_inspect(&w, &h, &state, png, 1024);
    if (error)
    {
        printf("error loading %s\n",filename);
        return 0;
    }

    GLenum glcolortype = GL_RGBA;
    LodePNGColorMode colormode = state.info_png.color;
//  unsigned bitdepth = colormode.bitdepth;
    LodePNGColorType colortype = colormode.colortype;
    switch(colortype)
    {
    /*
      case LCT_GREY:
        glcolortype = GL_LUMINANCE;
        error = lodepng_decode_memory(&image, &w, &h, png, pngsize, colortype, bitdepth);
        components = 1;
        break;
      case LCT_GREY_ALPHA:
        glcolortype = GL_LUMINANCE_ALPHA;
        error = lodepng_decode_memory(&image, &w, &h, png, pngsize, colortype, bitdepth);
        components = 2;
        break;
    */
    case LCT_RGB:
        glcolortype = GL_RGB;
        error = lodepng_decode24(&image, &w, &h, png, pngsize);
        components = 3;
        break;
    case LCT_RGBA:
        glcolortype = GL_RGBA;
        error = lodepng_decode32(&image, &w, &h, png, pngsize);
        components = 4;
        break;
    case LCT_PALETTE:
    default:
        glcolortype = GL_RGBA;
        error = lodepng_decode32(&image, &w, &h, png, pngsize);
        components = 4;
        break;
    }

    if(error)
    {
        printf("PNG decoder error %u: %s\n", error, lodepng_error_text(error));
        return 0;
    }

    free(png);

    // Texture size must be power of 2 (for some implementations of GLES)
    // Find next power of two
    size_t u2 = 1;
    while(u2 < w) u2 *= 2;
    size_t v2 = 1;
    while(v2 < h) v2 *= 2;

    // Make power of two version of the image.
    unsigned char *image2 = (unsigned char *)malloc(sizeof(unsigned char) * u2 * v2 * components);
    for(size_t y = 0; y < h; y++)
    {
        for(size_t x = 0; x < w; x++)
        {
            for(size_t c = 0; c < components; c++)
            {
                unsigned char val = image[components * w * y + components * x + c];
                image2[components * u2 * y + components * x + c] = val;
            }
        }
    }

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexImage2D(GL_TEXTURE_2D, 0, glcolortype, u2, v2, 0, glcolortype, GL_UNSIGNED_BYTE, image2);

    /*cleanup*/
    lodepng_state_cleanup(&state);
    free(image2);
    free(image);

    return texture;
}
