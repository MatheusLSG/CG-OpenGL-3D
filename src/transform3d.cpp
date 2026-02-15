#include "../lib/vec3.h"
#include "../lib/transform3d.h"
#include <GL/gl.h>


vec3 translacao3D(GLfloat dx, GLfloat dy, GLfloat dz,  vec3 ponto)
{
    return ponto + vec3(dx, dy, dz);
}

vec3 rotacao3Dx(GLfloat ang, vec3 ponto)
{
    GLfloat c = cos(ang*(M_PI / 180));
    GLfloat s = sin(ang*(M_PI / 180));
    return vec3(
        ponto.x(),
        ponto.y()*c - ponto.z()*s,
        ponto.y()*s + ponto.z()*c
    );
}

vec3 rotacao3Dy(GLfloat ang, vec3 ponto)
{
    GLfloat c = cos(ang*(M_PI / 180));
    GLfloat s = sin(ang*(M_PI / 180));
    return vec3(
        ponto.x()*c + ponto.z()*s,
        ponto.y(),
        -ponto.x()*s + ponto.z()*c
    );
}

vec3 rotacao3Dz(GLfloat ang, vec3 ponto)
{
    GLfloat c = cos(ang*(M_PI / 180));
    GLfloat s = sin(ang*(M_PI / 180));
    return vec3(
        ponto.x()*c - ponto.y()*s,
        ponto.x()*s + ponto.y()*c,
        ponto.z()
    );
}