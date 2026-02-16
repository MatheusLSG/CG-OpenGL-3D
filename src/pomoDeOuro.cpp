#include "../lib/pomoDeOuro.h"
#include "../lib/leitorDeSVG.h"
#include <GL/gl.h>
#include <GL/glut.h>
#include <cmath>

const GLfloat PomoDeOuro::RAIO_ESFERA = 3.f;

const GLfloat PomoDeOuro::AMBIENTE[4]  = { 0.25f, 0.25f, 0.3f, 1.f };
const GLfloat PomoDeOuro::DIFUSA[4]    = { 0.95f, 0.95f, 1.f, 1.f };
const GLfloat PomoDeOuro::ESPECULAR[4] = { 1.f, 1.f, 1.f, 1.f };

PomoDeOuro::PomoDeOuro() : ativo_(true) {
    posicao_[0] = 0.f;
    posicao_[1] = 30.f;
    posicao_[2] = 0.f;
    posicao_[3] = 1.f;
}

void PomoDeOuro::configurarLuzOpenGL() const {
    glLightfv(GL_LIGHT0, GL_AMBIENT,  AMBIENTE);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  DIFUSA);
    glLightfv(GL_LIGHT0, GL_SPECULAR, ESPECULAR);
}

void PomoDeOuro::atualizar(const DadosArenaSVG* dadosArena, GLfloat alturaArena) {
    if (!dadosArena || !dadosArena->ok) {
        posicao_[0] = 0.f;
        posicao_[1] = 30.f;
        posicao_[2] = 0.f;
        posicao_[3] = 1.f;
        return;
    }
    GLfloat t = (GLfloat)glutGet(GLUT_ELAPSED_TIME) * 0.001f;
    GLfloat r = (GLfloat)dadosArena->arena_r;
    GLfloat centro_x = (GLfloat)dadosArena->arena_cx;
    GLfloat centro_z = (GLfloat)dadosArena->arena_cy;  /* DadosArenaSVG: arena_cy = z no mundo 3D */
    GLfloat raio_orbita = 0.8f * r;
    GLfloat centro_y = 0.5f * alturaArena;
    GLfloat amp_y = 0.15f * r;
    const GLfloat omega_vertical = 1.3f;

    posicao_[0] = centro_x + raio_orbita * cosf(t);
    posicao_[2] = centro_z + raio_orbita * sinf(t);
    posicao_[1] = centro_y + amp_y * sinf(omega_vertical * t);
    posicao_[3] = 1.f;
}

void PomoDeOuro::aplicarLuz() const {
    if (ativo_) {
        glEnable(GL_LIGHT0);
        glLightfv(GL_LIGHT0, GL_POSITION, posicao_);
    } else {
        glDisable(GL_LIGHT0);
    }
}

void PomoDeOuro::desenhar() const {
    glPushMatrix();
    glTranslatef(posicao_[0], posicao_[1], posicao_[2]);
    GLfloat emiss[4] = { 0.7f, 0.75f, 1.f, 1.f };
    GLfloat amb[4]   = { 0.2f, 0.2f, 0.3f, 1.f };
    glMaterialfv(GL_FRONT, GL_EMISSION, emiss);
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, amb);
    glMaterialfv(GL_FRONT, GL_SPECULAR, emiss);
    glColor3f(0.9f, 0.9f, 1.f);
    glutSolidSphere(RAIO_ESFERA, 16, 16);
    GLfloat zero[4] = { 0.f, 0.f, 0.f, 1.f };
    glMaterialfv(GL_FRONT, GL_EMISSION, zero);
    glPopMatrix();
}

void PomoDeOuro::alternarAtivo() {
    ativo_ = !ativo_;
}
