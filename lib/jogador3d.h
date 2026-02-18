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

    GLfloat jogador_vy;  /**< Velocidade vertical (unidades/s); positivo = subindo. */
    GLfloat jogador_y_inicio_pulo; /**< Y do chão quando iniciou o pulo (limite de altura é relativo a isso). */
    int     jogador_movimento_ar;  /**< No ar: 1/2=frente, -1/-2=trás, 0=não mover (permite arco no pulo). */

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
        jogador_dir         {rotacao3Dy(ini_jogador_theta, vec3(0,0,1))},
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
        jogador_vy          {0},
        jogador_y_inicio_pulo{0},
        jogador_movimento_ar{0},
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

    /** @brief Ajusta a posição no plano XZ (usado pela resolução de colisões). */
    void set_pos_xz(GLfloat x, GLfloat z);

    /**
     * @brief Define posição (y=0) e rotação do corpo (ângulo em graus no plano XZ).
     * Usado no reset para recolocar o jogador na posição inicial encarando o inimigo.
     */
    void set_posicao_e_rotacao(vec3 pos, GLfloat theta_graus);

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
    /**
     * @brief Aplica o deslocamento do jogador conforme estado atual e delta de tempo (ms).
     * Usa JOGADOR_VELOCIDADE * (time_dif/1000) para ser independente da taxa de quadros.
     */
    void atualiza_movimento(GLdouble time_dif);
    /** @brief Para o movimento (estado PARADO). */
    void para();
    /** @brief Inicia o pulo (estado PULANDO) se não estiver no ar. */
    void pula();
    /** @brief Solta o pulo: se estiver subindo, começa a descer (altura variável). */
    void pula_soltar();
    /** @brief Retorna true se estiver PULANDO ou CAINDO. */
    bool esta_no_ar() const;
    /** @brief Aplica gravidade e pouso; t_dif em segundos, obstaculos e outro jogador para nível do chão. */
    void gravidade(GLfloat t_dif, const std::list<Obstaculo3d>& obstaculos, const Jogador3d* outro = nullptr);
    /** @brief Gira o corpo do jogador (ângulo em graus). */
    void gira_corpo(GLfloat theta_dif);
    /** @brief Gira a arma (ângulos theta e phi em graus). */
    void gira_arma(GLfloat theta_dif, GLfloat phi_dif);
    /** @brief Dispara um tiro (estado ATACANDO). */
    void atira();
    /** @brief Retorna true se este jogador colide (sobrepõe) com o inimigo no plano XZ. */
    bool verifica_colisao_inimigo(const Jogador3d& inimigo) const;
    /** @brief Retorna true se colide com algum obstáculo da lista (plano XZ). */
    bool verifica_colisao_obstaculos(const std::list<Obstaculo3d>& obstaculos) const;
    /** @brief Retorna true se está fora da arena (centro + raio no plano XZ). */
    bool verifica_colisao_arena(vec3 centro_arena, GLfloat raio_arena) const;

    /** @brief Corrige posição para ficar dentro da arena (atrator: moonwalk na parede). */
    void aplica_colisao_arena(vec3 centro_arena, GLfloat raio_arena);
    /** @brief Corrige posição para não penetrar pilastras (repulsores). */
    void aplica_colisao_obstaculos(const std::list<Obstaculo3d>& obstaculos);
    /** @brief Separa este jogador e o outro (repulsor mútuo). */
    void aplica_colisao_inimigo(Jogador3d& outro);

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
        jogador_vy = 0;
        jogador_y_inicio_pulo = 0;
        jogador_movimento_ar = 0;
    }
};

#endif // _JOGADOR_H_
