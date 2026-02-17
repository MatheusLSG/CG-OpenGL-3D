#include "../lib/obstaculo3d.h"
#include "../lib/config.h"
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

GLuint Obstaculo3d::idTexturaParedes_ = 0;

// Desenha um disco no plano XY (z=0) com normal (0,0,1), usando triângulos.
static void desenhaDisco(GLfloat raio, int numFatias)
{
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0.0f, 0.0f, 1.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    for (int i = 0; i <= numFatias; ++i) {
        GLfloat t = (GLfloat)i / (GLfloat)numFatias * 2.0f * (GLfloat)M_PI;
        GLfloat cx = std::cos(t) * raio;
        GLfloat sy = std::sin(t) * raio;
        glVertex3f(cx, sy, 0.0f);
    }
    glEnd();
}

// Desenha a superfície lateral de um cilindro ao longo do eixo Z (de z=0 a z=altura).
// Se texCoord true, envia coordenadas de textura para a lateral.
static void desenhaCilindroLateral(GLfloat raio, GLfloat altura, int numFatias, bool texCoord = false)
{
    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= numFatias; ++i) {
        GLfloat ang = (GLfloat)i / (GLfloat)numFatias * 2.0f * (GLfloat)M_PI;
        GLfloat cx = std::cos(ang);
        GLfloat sy = std::sin(ang);
        glNormal3f(cx, sy, 0.0f);
        if (texCoord) {
            GLfloat s = (GLfloat)i / (GLfloat)numFatias;
            glTexCoord2f(s, 0.0f); glVertex3f(raio * cx, raio * sy, 0.0f);
            glTexCoord2f(s, 1.0f); glVertex3f(raio * cx, raio * sy, altura);
        } else {
            glVertex3f(raio * cx, raio * sy, 0.0f);
            glVertex3f(raio * cx, raio * sy, altura);
        }
    }
    glEnd();
}

void Obstaculo3d::desenha()
{
    glPushMatrix();

    // obstaculo_pos = centro da base do cilindro; eixo ao longo Y (altura)
    glTranslatef(obstaculo_pos.x(), obstaculo_pos.y(), obstaculo_pos.z());
    // Cilindro desenhado ao longo de Z; rotacionamos para ter eixo Y
    glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);

    if (idTexturaParedes_ != 0) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, idTexturaParedes_);
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        glColor3f(1.f, 1.f, 1.f);
        GLfloat mat[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
        glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat);
    } else {
        glDisable(GL_TEXTURE_2D);
        glColor3f(PRETO.x(), PRETO.y(), PRETO.z());
        GLfloat mat[4] = { PRETO.x(), PRETO.y(), PRETO.z(), 1.0f };
        glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat);
    }

    desenhaCilindroLateral(obstaculo_raio, obstaculo_altura, PONTOS_POR_ELIPSE, (idTexturaParedes_ != 0));

    if (idTexturaParedes_ != 0)
        glDisable(GL_TEXTURE_2D);

    if (desenha_tampa_superior) {
        glTranslatef(0.0f, 0.0f, obstaculo_altura);
        glRotatef(180.0f, 1.0f, 0.0f, 0.0f);
        glColor3f(obstaculo_cor.x(), obstaculo_cor.y(), obstaculo_cor.z());
        GLfloat mat[4] = { obstaculo_cor.x(), obstaculo_cor.y(), obstaculo_cor.z(), 1.0f };
        glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat);
        desenhaDisco(obstaculo_raio, PONTOS_POR_ELIPSE);
    }

    glPopMatrix();
}

vec3 Obstaculo3d::direcao_forca(const vec3& ponto) const
{
    // Projeção no plano XZ (horizontal) para arena e pilastras
    GLfloat dx = ponto.x() - obstaculo_pos.x();
    GLfloat dz = ponto.z() - obstaculo_pos.z();
    GLfloat len = std::sqrt(dx * dx + dz * dz);

    const GLfloat eps = 1e-6f;
    if (len < eps)
        return vec3(0.0f, 0.0f, 0.0f);

    GLfloat ux = dx / len;
    GLfloat uz = dz / len;

    if (tipo_obstaculo == OBSTACULO_REPULSOR)
        return vec3(ux, 0.0f, uz);  // empurra para longe do centro
    else
        return vec3(-ux, 0.0f, -uz); // atrator: puxa em direção ao centro
}
