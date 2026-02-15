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
// momento confia
class Tiro;

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
    
    std::list<Tiro*> tiros;

public:
    Jogador3d() {};
    Jogador3d(  GLfloat ini_jogador_raio,
                GLfloat ini_jogador_theta, 
                vec3 ini_jogador_pos,
                cor ini_jogador_tiro_cor,
                meshes animacoes) :
        jogador_vida            {VIDAS_INICIAL},
        jogador_raio            {ini_jogador_raio},
        jogador_pos             {ini_jogador_pos},
        jogador_dir             {rotacao3Dz(ini_jogador_theta, vec3(1,0,0))},
        Jogador_theta           {},
        jogador_modelo          {animacoes},
        jogador_anim_atual      {PARADO},
        jogador_arma_pos        {ini_jogador_pos},
        jogador_arma_dir        {},
        jogador_arma_tetha      {},
        jogador_arma_phi        {},
        jogador_tiro_cor        {ini_jogador_tiro_cor},
        tiros                   {std::list<Tiro*>()}
    {};

    GLfloat pos_x() const {return jogador_pos.x();};
    GLfloat pos_y() const {return jogador_pos.y();};
    GLfloat pos_z() const {return jogador_pos.z();};
    vec3 pos() const {return jogador_pos;};
    GLfloat raio() const {return jogador_raio;};
    GLint vidas() const {return jogador_vida;};
    
    std::list<Tiro*> &retorna_tiros() {return tiros;};

    void atualiza_animacao();
    void desenha_jogador();
    void desenha_tiros();
    void move(GLfloat s_dif);
    void para();
    void pula();
    void gravidade(GLfloat t_dif);
    void gira_corpo(GLfloat theta_dif);
    void gira_arma(GLfloat theta_dif, GLfloat phi_dif);
    void atira();
    bool verifica_colisao_inimigo(const Jogador3d& inimigo);
    bool verifica_colisao_obstaculos(const std::list<Obstaculo3d>& obstaculos);
    bool verifica_colisao_arena();
    
    void dano();

};




#endif // _JOGADOR_H_
