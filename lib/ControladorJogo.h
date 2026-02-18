#ifndef CONTROLADORJOGO_H
#define CONTROLADORJOGO_H

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <cstring>

#include "config.h"
#include "placar.h"
#include "jogador3d.h"

/**
 * @brief Estado da partida: em andamento ou game over.
 */
enum EstadoJogo {
    EM_PARTIDA,
    GAME_OVER
};

/**
 * @brief Controla o fluxo do jogo: atualização de estado, fim de jogo, HUD e mensagem de vitória.
 */
class ControladorJogo {
    Jogador3d& jEsq;
    Jogador3d& jDir;
    Placar& placar;
    EstadoJogo estado;
    int vencedor;

public:
    /**
     * @brief Constrói o controlador com referências ao jogador esquerdo, direito e ao placar.
     */
    ControladorJogo(Jogador3d& jogadorEsq,
                    Jogador3d& jogadorDir,
                    Placar& p)
        : jEsq(jogadorEsq),
          jDir(jogadorDir),
          placar(p),
          estado(EM_PARTIDA),
          vencedor(0)
    {}

    /**
     * @brief Retorna o estado atual da partida.
     * @return EM_PARTIDA ou GAME_OVER.
     */
    EstadoJogo getEstado() const { return estado; }

    /**
     * @brief Atualiza o estado do jogo (tiros, colisões, verificação de fim).
     * @param dt Delta de tempo (segundos).
     */
    void atualizar(double dt)
    {
        if (estado == GAME_OVER)
            return;

        if (jEsq.vidas() <= 0) {
            estado = GAME_OVER;
            vencedor = 2;
            jDir.ganha();   /* Draco ganha: animação de vitória */
        } else if (jDir.vidas() <= 0) {
            estado = GAME_OVER;
            vencedor = 1;
            jEsq.ganha();   /* Harry ganha: animação de vitória */
        }
    }

    /**
     * @brief Reinicia a partida: estado, vidas dos jogadores e placar.
     */
    void resetarJogo()
    {
        estado = EM_PARTIDA;
        vencedor = 0;
        jEsq.resetar_vidas();
        jDir.resetar_vidas();
        placar.resetar();
    }

    /**
     * @brief Desenha o HUD (placar de vidas) e, se game over, a mensagem de vitória.
     */
    void desenharHUD()
    {
        placar.desenhar(jEsq, jDir);

        if (estado == GAME_OVER) {
            GLint viewport[4];
            glGetIntegerv(GL_VIEWPORT, viewport);

            glDisable(GL_LIGHTING);
            glDisable(GL_ALPHA_TEST);
            glDisable(GL_DEPTH_TEST);

            glViewport(0, 0, JANELA_LARGURA, JANELA_ALTURA);

            glMatrixMode(GL_PROJECTION);
            glPushMatrix();
            glLoadIdentity();
            gluOrtho2D(0, JANELA_LARGURA, 0, JANELA_ALTURA);

            glMatrixMode(GL_MODELVIEW);
            glPushMatrix();
            glLoadIdentity();

            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glColor4f(0.0f, 0.0f, 0.0f, 0.75f);
            glBegin(GL_QUADS);
                glVertex2f(0.0f,            0.0f);
                glVertex2f(JANELA_LARGURA,  0.0f);
                glVertex2f(JANELA_LARGURA,  JANELA_ALTURA);
                glVertex2f(0.0f,            JANELA_ALTURA);
            glEnd();

            const char* msg = (vencedor == 1) ? "Harry Ganhou" : "Draco Ganhou";

            void* fonteStroke = GLUT_STROKE_ROMAN;
            const GLfloat escalaTexto = 0.4f;
            const GLfloat espessuraLinha = 3.0f;

            glLineWidth(espessuraLinha);

            int larguraStroke = 0;
            for (const char* c = msg; *c; ++c) {
                larguraStroke += glutStrokeWidth(fonteStroke, *c);
            }

            GLfloat larguraTexto = larguraStroke * escalaTexto;
            GLfloat alturaTexto  = 100.0f * escalaTexto;

            GLfloat x = (JANELA_LARGURA - larguraTexto) / 2.0f;
            GLfloat y = (JANELA_ALTURA - alturaTexto) / 2.0f;

            glColor3f(1.0f, 1.0f, 1.0f);
            glTranslatef(x, y, 0.0f);
            glScalef(escalaTexto, escalaTexto, 1.0f);
            for (const char* c = msg; *c; ++c) {
                glutStrokeCharacter(fonteStroke, *c);
            }

            glDisable(GL_BLEND);

            glPopMatrix();
            glMatrixMode(GL_PROJECTION);
            glPopMatrix();
            glMatrixMode(GL_MODELVIEW);

            glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
            glEnable(GL_DEPTH_TEST);
            glEnable(GL_LIGHTING);
        }
    }
};

#endif // CONTROLADORJOGO_H