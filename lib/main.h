#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <math.h>
#include <cstring>
#include <stdlib.h> 

//Funcao auxiliar para normalizar um vetor a/|a|
void normalize(float a[3])
{
    double norm = sqrt(a[0]*a[0]+a[1]*a[1]+a[2]*a[2]); 
    a[0] /= norm;
    a[1] /= norm;
    a[2] /= norm;
}

//Funcao auxiliar para fazer o produto vetorial entre dois vetores a x b = out
void cross(float a[3], float b[3], float out[3])
{
    out[0] = a[1]*b[2] - a[2]*b[1];
    out[1] = a[2]*b[0] - a[0]*b[2];
    out[2] = a[0]*b[1] - a[1]*b[0];
}

//Aplica a transformacao que coloca o sistema de coordendas local em uma posicao 
//do mundo definida por dois pontos a e b e um vetor up. O ponto b sera a origem,
//o vetor BA = A - B sera determinara o novo y, o vetor up definira a orientacao
//do novo z em torno de BA.
//A matriz de transformacao no opengl eh armazanada transposta: 
//m[4][4] = {Xx, Xy, Xz, 0.0, Yx, Yy, Yz, 0.0, Zx, Zy, Zz, 0.0, Tx, Ty, Tz, 1.0}
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

	//COLOQUE SEU CODIGO AQUI
    //y
    y[0] = ax-bx;
    y[1] = ay-by;
    y[2] = az-bz;
    normalize(&y[0]);
    
    //pre_z
    z[0] = upx;
    z[1] = upy;
    z[2] = upz;
    normalize(&z[0]);
    
    //x
    cross(&y[0], &z[0], &x[0]);
    normalize(&x[0]);
    
    //z
    cross(&x[0], &y[0], &z[0]);
    normalize(&z[0]);

    //loadMat
    for(int i = 0; i<3;i++)  m[0][i] = x[i];
    for(int i = 0; i<3;i++)  m[1][i] = y[i];
    for(int i = 0; i<3;i++)  m[2][i] = z[i];
    
    m[3][0] = bx;
    m[3][1] = by;
    m[3][2] = bz; 

    //
    glMultMatrixf(&m[0][0]);
}