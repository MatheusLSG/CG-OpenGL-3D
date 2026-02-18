#ifndef PLACAR_H
#define PLACAR_H

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <cstring>

#include "config.h"
#include "jogador3d.h"
#include "desenho2d.h"

/**
 * @brief Placar de vidas (corações 2D) e animação de explosão ao levar dano.
 */
class Placar {
    int hitsEsquerda;
    int hitsDireita;

    bool   explosaoEsqAtiva;
    int    explosaoEsqInicioMs;
    GLfloat explosaoEsqX;
    GLfloat explosaoEsqY;
    GLfloat explosaoEsqRaio;

    bool   explosaoDirAtiva;
    int    explosaoDirInicioMs;
    GLfloat explosaoDirX;
    GLfloat explosaoDirY;
    GLfloat explosaoDirRaio;

public:
    Placar()
        : hitsEsquerda(0),
          hitsDireita(0),
          explosaoEsqAtiva(false),
          explosaoEsqInicioMs(0),
          explosaoEsqX(0.0f),
          explosaoEsqY(0.0f),
          explosaoEsqRaio(0.0f),
          explosaoDirAtiva(false),
          explosaoDirInicioMs(0),
          explosaoDirX(0.0f),
          explosaoDirY(0.0f),
          explosaoDirRaio(0.0f)
    {}

    void registrarHitEsquerda() { hitsEsquerda++; }
    void registrarHitDireita()  { hitsDireita++;  }

    int getHitsEsquerda() const { return hitsEsquerda; }
    int getHitsDireita()  const { return hitsDireita;  }

    /**
     * @brief Notifica que o jogador da esquerda levou dano; inicia animação de explosão no coração correspondente.
     * @param vidasAntes Número de vidas do jogador antes de aplicar o dano.
     */
    void notificarDanoEsquerda(int vidasAntes)
    {
        if (vidasAntes <= 0) return;

        const GLfloat raioCoracao = 12.0f;
        const GLfloat margemX = 10.0f;
        const GLfloat margemY = 10.0f;
        const GLfloat espacamento = 4.0f;

        int indice = vidasAntes - 1;

        explosaoEsqRaio = raioCoracao;
        explosaoEsqX = margemX + raioCoracao + indice * (2.0f * raioCoracao + espacamento);
        explosaoEsqY = JANELA_ALTURA - margemY - raioCoracao;

        explosaoEsqAtiva = true;
        explosaoEsqInicioMs = glutGet(GLUT_ELAPSED_TIME);
    }

    /**
     * @brief Notifica que o jogador da direita levou dano; inicia animação de explosão no coração correspondente.
     * @param vidasAntes Número de vidas do jogador antes de aplicar o dano.
     */
    void notificarDanoDireita(int vidasAntes)
    {
        if (vidasAntes <= 0) return;

        const GLfloat raioCoracao = 12.0f;
        const GLfloat margemX = 10.0f;
        const GLfloat margemY = 10.0f;
        const GLfloat espacamento = 4.0f;

        int indice = vidasAntes - 1;

        explosaoDirRaio = raioCoracao;
        explosaoDirX = JANELA_LARGURA - margemX - raioCoracao - indice * (2.0f * raioCoracao + espacamento);
        explosaoDirY = JANELA_ALTURA - margemY - raioCoracao;

        explosaoDirAtiva = true;
        explosaoDirInicioMs = glutGet(GLUT_ELAPSED_TIME);
    }

    void resetar()
    {
        hitsEsquerda = 0;
        hitsDireita  = 0;

        explosaoEsqAtiva   = false;
        explosaoDirAtiva   = false;
        explosaoEsqInicioMs = 0;
        explosaoDirInicioMs = 0;
    }

    /**
     * @brief Desenha o HUD de vidas (corações) e as animações de explosão em projeção 2D.
     * @param jogadorEsq Jogador da esquerda (Harry).
     * @param jogadorDir Jogador da direita (Draco).
     */
    void desenhar(const Jogador3d& jogadorEsq,
                  const Jogador3d& jogadorDir)
    {
        GLint viewport[4];
        glGetIntegerv(GL_VIEWPORT, viewport);

        glViewport(0, 0, JANELA_LARGURA, JANELA_ALTURA);

        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        gluOrtho2D(0, JANELA_LARGURA, 0, JANELA_ALTURA);

        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();

        glDisable(GL_DEPTH_TEST);
        glDisable(GL_LIGHTING);

        {
            const int vidasEsq = jogadorEsq.vidas();
            const GLfloat raioCoracao = 12.0f;
            const GLfloat margemX = 10.0f;
            const GLfloat margemY = 10.0f;
            const GLfloat espacamento = 4.0f;

            glColor3f(1.0f, 0.0f, 0.0f);

            for (int i = 0; i < vidasEsq; ++i) {
                GLfloat cx = margemX + raioCoracao + i * (2.0f * raioCoracao + espacamento);
                GLfloat cy = JANELA_ALTURA - margemY - raioCoracao;
                desenhaCoracao2D(cx, cy, raioCoracao);
            }
        }

        {
            const int vidasDir = jogadorDir.vidas();
            const GLfloat raioCoracao = 12.0f;
            const GLfloat margemX = 10.0f;
            const GLfloat margemY = 10.0f;
            const GLfloat espacamento = 4.0f;

            glColor3f(0.0f, 1.0f, 0.0f);

            for (int i = 0; i < vidasDir; ++i) {
                GLfloat cx = JANELA_LARGURA - margemX - raioCoracao - i * (2.0f * raioCoracao + espacamento);
                GLfloat cy = JANELA_ALTURA - margemY - raioCoracao;
                desenhaCoracao2D(cx, cy, raioCoracao);
            }
        }

        const int DURACAO_FRAME_MS = 80;
        int tempoAtual = glutGet(GLUT_ELAPSED_TIME);

        if (explosaoEsqAtiva) {
            int elapsed = tempoAtual - explosaoEsqInicioMs;
            int frame = elapsed / DURACAO_FRAME_MS;
            if (frame > 3) {
                explosaoEsqAtiva = false;
            } else {
                glColor3f(1.0f, 0.0f, 0.0f);
                desenhaCoracaoExplodindo2D(explosaoEsqX, explosaoEsqY, explosaoEsqRaio, frame);
            }
        }

        if (explosaoDirAtiva) {
            int elapsed = tempoAtual - explosaoDirInicioMs;
            int frame = elapsed / DURACAO_FRAME_MS;
            if (frame > 3) {
                explosaoDirAtiva = false;
            } else {
                glColor3f(0.0f, 1.0f, 0.0f);
                desenhaCoracaoExplodindo2D(explosaoDirX, explosaoDirY, explosaoDirRaio, frame);
            }
        }

        glEnable(GL_LIGHTING);
        glEnable(GL_DEPTH_TEST);

        glPopMatrix();
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);

        glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
    }
};

#endif // PLACAR_H
