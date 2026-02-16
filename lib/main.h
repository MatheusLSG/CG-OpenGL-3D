#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <math.h>
#include <cstring>
#include <stdlib.h>

/**
 * @brief Normaliza o vetor in-place (a passa a ter norma 1).
 * @param a Vetor de 3 floats (x, y, z).
 */
void normalize(float a[3])
{
    double norm = sqrt(a[0]*a[0]+a[1]*a[1]+a[2]*a[2]); 
    a[0] /= norm;
    a[1] /= norm;
    a[2] /= norm;
}

/**
 * @brief Calcula o produto vetorial a x b e armazena em out.
 * @param a Primeiro vetor.
 * @param b Segundo vetor.
 * @param out Resultado (não precisa ser inicializado).
 */
void cross(float a[3], float b[3], float out[3])
{
    out[0] = a[1]*b[2] - a[2]*b[1];
    out[1] = a[2]*b[0] - a[0]*b[2];
    out[2] = a[0]*b[1] - a[1]*b[0];
}

/**
 * @brief Aplica a transformação que coloca o sistema de coordenadas local: origem em B, eixo Y na direção A-B, Z orientado por up.
 * @param ax, ay, az Ponto A (alvo do eixo Y).
 * @param bx, by, bz Ponto B (origem do sistema).
 * @param upx, upy, upz Vetor up (orientação do eixo Z).
 */
void ChangeCoordSys(
        GLdouble ax, GLdouble ay, GLdouble az, 
        GLdouble bx, GLdouble by, GLdouble bz, 
        GLdouble upx, GLdouble upy, GLdouble upz)
{
    float x[3], y[3], z[3];
    GLfloat m[4][4] = { 1,0,0,0,
                        0,1,0,0,
                        0,0,1,0,
                        0,0,0,1};

    y[0] = ax-bx;
    y[1] = ay-by;
    y[2] = az-bz;
    normalize(&y[0]);

    z[0] = upx;
    z[1] = upy;
    z[2] = upz;
    normalize(&z[0]);

    cross(&y[0], &z[0], &x[0]);
    normalize(&x[0]);

    cross(&x[0], &y[0], &z[0]);
    normalize(&z[0]);

    for(int i = 0; i<3;i++)  m[0][i] = x[i];
    for(int i = 0; i<3;i++)  m[1][i] = y[i];
    for(int i = 0; i<3;i++)  m[2][i] = z[i];
    
    m[3][0] = bx;
    m[3][1] = by;
    m[3][2] = bz;

    glMultMatrixf(&m[0][0]);
}