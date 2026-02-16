#if !defined(_JOGADOR_H_)
#define _JOGADOR_H_

#include <GL/gl.h>
#include <list>

#include "../lib/config.h"
#include "../lib/vec3.h"
#include "../lib/tiro3d.h"
#include "../lib/transform3d.h"
#include "../lib/obstaculo3d.h"
#include "../lib/objloader.h"

class Tiro;

/**
 * @brief Jogador 3D: posição, direção, modelo animado, tiros e colisões.
 */
class Jogador3d
{
    GLint   jogador_vida;
    GLfloat jogador_raio;

    vec3    jogador_pos;
    vec3    jogador_dir;
    GLfloat Jogador_theta;

    meshes  jogador_modelo;
    Estado  jogador_estado_atual;
    Estado  jogador_anim_atual;

    vec3    jogador_arma_pos;
    vec3    jogador_arma_dir;
    GLfloat jogador_arma_tetha;
    GLfloat jogador_arma_phi;

    cor     jogador_tiro_cor;

    int     jogador_flag_anim;
    int     jogador_inv;

    std::list<Tiro*> tiros;

public:
    Jogador3d() {}
    Jogador3d(GLfloat ini_jogador_raio,
              GLfloat ini_jogador_theta,
              vec3 ini_jogador_pos,
              cor ini_jogador_tiro_cor,
              meshes animacoes) :
        jogador_vida        {VIDAS_INICIAL},
        jogador_raio        {ini_jogador_raio},
        jogador_pos         {ini_jogador_pos},
        jogador_dir         {rotacao3Dz(ini_jogador_theta, vec3(1,0,0))},
        Jogador_theta       {ini_jogador_theta},
        jogador_modelo      {animacoes},
        jogador_estado_atual{PARADO},
        jogador_anim_atual  {PARADO},
        jogador_arma_pos    {ini_jogador_pos},
        jogador_arma_dir    {},
        jogador_arma_tetha  {},
        jogador_arma_phi    {},
        jogador_tiro_cor    {ini_jogador_tiro_cor},
        jogador_flag_anim   {1},
        jogador_inv         {0},
        tiros               {std::list<Tiro*>()}
    {}

    GLfloat pos_x() const   { return jogador_pos.x(); }
    GLfloat pos_y() const   { return jogador_pos.y(); }
    GLfloat pos_z() const   { return jogador_pos.z(); }
    void    ganha()         { jogador_estado_atual = GANHAR;  jogador_inv = 1;}
    /**
     * @brief Posição 3D do jogador.
     * @return Vetor posição (x, y, z).
     */
    vec3 pos() const { return jogador_pos; }
    GLfloat raio() const { return jogador_raio; }
    GLfloat altura() const { return JOGADOR_ALTURA; }
    GLint vidas() const { return jogador_vida; }

    std::list<Tiro*>& retorna_tiros() { return tiros; }

    /**
     * @brief Avança o frame da animação conforme o estado atual (PARADO, ANDANDO, etc.).
     */
    void atualiza_animacao();
    /** @brief Desenha o modelo do jogador na posição e rotação atuais. */
    void desenha_jogador();
    /** @brief Desenha os tiros ativos do jogador. */
    void desenha_tiros();
    /**
     * @brief Define movimento: andar ou correr, frente ou trás.
     * @param s_dif 1 = andar frente, 2 = correr frente, -1 = andar trás, -2 = correr trás.
     */
    void move(GLfloat s_dif);
    /** @brief Para o movimento (estado PARADO). */
    void para();
    /** @brief Inicia o pulo (estado PULANDO) se não estiver caindo. */
    void pula();
    /** @brief Aplica gravidade; delta de tempo em segundos. */
    void gravidade(GLfloat t_dif);
    /** @brief Gira o corpo do jogador (ângulo em graus). */
    void gira_corpo(GLfloat theta_dif);
    /** @brief Gira a arma (ângulos theta e phi em graus). */
    void gira_arma(GLfloat theta_dif, GLfloat phi_dif);
    /** @brief Dispara um tiro (estado ATACANDO). */
    void atira();
    /** @brief Retorna true se este jogador colide com o inimigo. */
    bool verifica_colisao_inimigo(const Jogador3d& inimigo);
    /** @brief Retorna true se colide com algum obstáculo da lista. */
    bool verifica_colisao_obstaculos(const std::list<Obstaculo3d>& obstaculos);
    /** @brief Retorna true se está fora da arena (colisão com borda). */
    bool verifica_colisao_arena();

    /**
     * @brief Reduz uma vida do jogador. O placar deve ser notificado em seguida.
     */
    void dano();
    void resetar_vidas() 
    { 
        jogador_vida = VIDAS_INICIAL;  
        jogador_flag_anim = 1; 
        jogador_modelo.drawInit(PARADO);
        jogador_estado_atual = PARADO;
        jogador_inv = 0;
    }
};

#endif // _JOGADOR_H_
