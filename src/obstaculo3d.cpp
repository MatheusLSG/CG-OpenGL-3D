#include "../lib/obstaculo3d.h"
#include "../lib/config.h"
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

GLuint Obstaculo3d::idTexturaParedes_ = 0;

void Obstaculo3d::desenha()
{
    glPushMatrix();

    // obstaculo_pos = centro da base do cilindro; eixo ao longo Y (altura)
    glTranslatef(obstaculo_pos.x(), obstaculo_pos.y(), obstaculo_pos.z());
    // gluCylinder usa eixo Z; rotacionamos para ter eixo Y
    glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);

    GLUquadric* quad = gluNewQuadric();
    gluQuadricNormals(quad, GLU_SMOOTH);

    if (idTexturaParedes_ != 0) {
        gluQuadricTexture(quad, GL_TRUE);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, idTexturaParedes_);
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        glColor3f(1.f, 1.f, 1.f);
        GLfloat mat[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
        glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat);
    } else {
        // Sem textura: desliga textura e não gera coords (drivers fracos/WSL podem falhar com tex coords e nenhuma textura).
        gluQuadricTexture(quad, GL_FALSE);
        glDisable(GL_TEXTURE_2D);
        glColor3f(PRETO.x(), PRETO.y(), PRETO.z());
        GLfloat mat[4] = { PRETO.x(), PRETO.y(), PRETO.z(), 1.0f };
        glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat);
    }

    gluCylinder(quad, obstaculo_raio, obstaculo_raio, obstaculo_altura, PONTOS_POR_ELIPSE, 4);

    if (idTexturaParedes_ != 0)
        glDisable(GL_TEXTURE_2D);

    if (desenha_tampa_superior) {
        glTranslatef(0.0f, 0.0f, obstaculo_altura);
        glRotatef(180.0f, 1.0f, 0.0f, 0.0f);
        glColor3f(obstaculo_cor.x(), obstaculo_cor.y(), obstaculo_cor.z());
        GLfloat mat[4] = { obstaculo_cor.x(), obstaculo_cor.y(), obstaculo_cor.z(), 1.0f };
        glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat);
        gluQuadricTexture(quad, GL_FALSE);
        gluDisk(quad, 0.0, obstaculo_raio, PONTOS_POR_ELIPSE, 1);
        gluQuadricTexture(quad, GL_TRUE);
    }

    gluDeleteQuadric(quad);

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
