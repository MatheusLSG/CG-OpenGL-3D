#if !defined(_TIRO_H_)
#define _TIRO_H_


#include <GL/gl.h>
#include <GL/glu.h>
#include "../lib/vec3.h"
#include "../lib/jogador3d.h"
#include "../lib/transform3d.h"
#include "../lib/obstaculo3d.h"
#include "../lib/config.h"

class Jogador3d;

/**
 * @brief Projétil com posição, direção e tempo de vida; verificação de colisão com jogador, obstáculos e arena.
 */
class Tiro3d {
    vec3 tiro_pos_ini; 
    vec3 tiro_pos_atual;
    vec3 tiro_dir;
    GLfloat tiro_raio;
    GLfloat tiro_tempo_atual;
    GLfloat tiro_tempo_limite;
    cor  tiro_cor;
    Jogador3d* jogador;
public:
    /**
     * @brief Constrói um tiro na posição inicial com ângulo e raio dados.
     * @param pos_ini Posição inicial no mundo.
     * @param ang Ângulo de disparo (graus).
     * @param raio Raio do tiro (colisão).
     * @param cor Cor do tiro.
     * @param jogador Ponteiro para o jogador que disparou.
     */
    Tiro3d(vec3 pos_ini, GLfloat ang, GLfloat raio, cor cor, Jogador3d* jogador) :
        tiro_pos_ini        {pos_ini}, 
        tiro_pos_atual      {pos_ini},
        tiro_dir            {},
        tiro_raio           {raio},
        tiro_tempo_atual    {0},
        tiro_tempo_limite   {TIRO_TEMPO_LIMITE},
        tiro_cor            {cor},
        jogador             {jogador}
    {}; 

    GLfloat pos_x() const {return tiro_pos_atual.x();};
    GLfloat pos_y() const {return tiro_pos_atual.y();};
    GLfloat raio() const {return tiro_raio;};
    vec3 pos() const {return tiro_pos_atual;}

    /**
     * @brief Desenha o tiro na posição atual.
     */
    void desenha();

    /**
     * @brief Move o tiro na direção atual.
     * @param dist Distância a percorrer.
     */
    void move(GLfloat dist);

    /**
     * @brief Atualiza o tempo de vida do tiro; retorna false se expirou.
     * @param t_dif Delta de tempo.
     * @return true se o tiro ainda é válido.
     */
    bool valido(GLfloat t_dif);

    /**
     * @brief Verifica colisão com um jogador.
     * @param jogador Jogador alvo.
     * @return true se houve colisão.
     */
    bool verifica_colisao_jogador(Jogador3d& jogador);
    /**
     * @brief Verifica colisão com a lista de obstáculos.
     * @param obstaculos Lista de obstáculos.
     * @return true se houve colisão.
     */
    bool verifica_colisao_obstaculos(const std::list<Obstaculo3d>& obstaculos);
    /**
     * @brief Verifica colisão com a arena (cilindro atrator).
     * @return true se houve colisão.
     */
    bool verifica_colisao_arena();
};

#endif // _TIRO_H_
