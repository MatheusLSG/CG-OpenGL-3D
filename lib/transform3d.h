#if !defined(_TRANSFORM_H_)
#define _TRANSFORM_H_

#include "../lib/vec3.h"
#include <GL/gl.h>

vec3 translacao3D(GLfloat dx, GLfloat dy, GLfloat dz, vec3 ponto);

vec3 rotacao3Dx(GLfloat ang, vec3 ponto);

vec3 rotacao3Dy(GLfloat ang, vec3 ponto);

vec3 rotacao3Dz(GLfloat ang, vec3 ponto);

#endif // _TRANSFORM_H_
