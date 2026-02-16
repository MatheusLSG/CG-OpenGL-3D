#if !defined(POMO_DE_OURO_H)
#define POMO_DE_OURO_H

#include <GL/gl.h>
#include <GL/glut.h>

struct DadosArenaSVG;

/**
 * @brief Única luz do jogo: o Pomo de Ouro, uma esfera luminosa que voa no meio da arena.
 *        Órbita circular com oscilação em Y. Toggle com tecla N (liga/desliga; quando
 *        desligado, a cena fica totalmente escura).
 */
class PomoDeOuro {
public:
    PomoDeOuro();

    /**
     * @brief Configura GL_LIGHT0 (ambiente, difusa, especular). Chamar uma vez após glEnable(GL_LIGHTING).
     */
    void configurarLuzOpenGL() const;

    /**
     * @brief Atualiza a posição do pomo (órbita no plano XZ + oscilação em Y). Chamar antes de desenhar a cena.
     * @param dadosArena Dados da arena (centro e raio); pode ser nullptr se não houver arena.
     * @param alturaArena Altura total da arena (eixo Y); o ponto médio do pomo ficará em alturaArena/2.
     */
    void atualizar(const DadosArenaSVG* dadosArena, GLfloat alturaArena);

    /**
     * @brief Aplica a luz no estado atual: habilita GL_LIGHT0 e define posição, ou desabilita se inativo.
     */
    void aplicarLuz() const;

    /**
     * @brief Desenha a esfera do Pomo de Ouro na posição atual.
     */
    void desenhar() const;

    /**
     * @brief Alterna entre luz ligada e desligada (tecla N).
     */
    void alternarAtivo();

    /**
     * @brief Retorna true se a luz está ligada.
     * @return true quando ativo, false quando desligado.
     */
    bool estaAtivo() const { return ativo_; }

    /**
     * @brief Retorna o ponteiro para a posição (x, y, z, w) para uso com glLightfv(GL_POSITION, ...).
     * @return Array de 4 GLfloat (posição da luz).
     */
    const GLfloat* posicao() const { return posicao_; }

private:
    GLfloat posicao_[4];
    bool ativo_;
    static const GLfloat RAIO_ESFERA;
    static const GLfloat AMBIENTE[4];
    static const GLfloat DIFUSA[4];
    static const GLfloat ESPECULAR[4];
};

#endif // POMO_DE_OURO_H
