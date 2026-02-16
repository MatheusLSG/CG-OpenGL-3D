#if !defined(_OBSTACULO_H_)
#define _OBSTACULO_H_

#include <GL/gl.h>
#include <GL/glu.h>
#include "config.h"
#include "vec3.h"

/**
 * @brief Tipo de obstáculo: repulsor empurra para longe do centro; atrator puxa para o centro.
 */
enum TipoObstaculo {
    OBSTACULO_REPULSOR,
    OBSTACULO_ATRATOR
};

class Obstaculo3d {
    vec3   obstaculo_pos;
    GLfloat obstaculo_raio;
    GLfloat obstaculo_altura;
    cor    obstaculo_cor;
    TipoObstaculo tipo_obstaculo;
    bool   desenha_tampa_superior;

    static GLuint idTexturaParedes_;

public:
    /**
     * @brief Define a textura difusa usada nas paredes (arena e pilastras).
     * @param id ID da textura OpenGL; 0 para desenhar sem textura.
     */
    static void setTexturaParedes(GLuint id) { idTexturaParedes_ = id; }

    /**
     * @brief Constrói obstáculo cilíndrico (eixo Y).
     * @param pos_ini Centro da base no plano XZ.
     * @param raio Raio do cilindro.
     * @param c Cor.
     * @param altura Altura ao longo do eixo Y.
     * @param tipo OBSTACULO_REPULSOR ou OBSTACULO_ATRATOR.
     * @param desenha_tampa false deixa teto aberto (ex.: arena).
     */
    Obstaculo3d(vec3 pos_ini, GLfloat raio, cor c, GLfloat altura, TipoObstaculo tipo, bool desenha_tampa = true)
        : obstaculo_pos   {pos_ini},
          obstaculo_raio  {raio},
          obstaculo_altura{altura},
          obstaculo_cor   {c},
          tipo_obstaculo  {tipo},
          desenha_tampa_superior{desenha_tampa}
    {}

    GLfloat raio() const { return obstaculo_raio; }
    GLfloat altura() const { return obstaculo_altura; }
    vec3 pos() const { return obstaculo_pos; }
    TipoObstaculo tipo() const { return tipo_obstaculo; }
    cor cor_obstaculo() const { return obstaculo_cor; }

    /**
     * @brief Desenha o obstáculo como cilindro com eixo Y.
     */
    void desenha();

    /**
     * @brief Retorna a direção unitária da força aplicada a um ponto (plano XZ). Repulsor empurra para fora; atrator puxa para o centro.
     * @param ponto Ponto no mundo (Y ignorado para arena/pilastras).
     * @return Vetor direção unitária.
     */
    vec3 direcao_forca(const vec3& ponto) const;
};

#endif // _OBSTACULO_H_
