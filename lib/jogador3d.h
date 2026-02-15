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
    GLint   jogador_flag_andando;
    GLfloat jogador_tempo_andando;
    GLfloat jogador_raio;

    vec3    jogador_pos;
    vec3    jogador_dir;
    
    cor     jogador_tiro_cor;  
    
    mesh    jogador_mesh;

    GLfloat hit_box;
    
    std::list<Tiro*> tiros;

public:
    Jogador3d(GLfloat ini_cabeca_raio, 
            vec3 ini_jogador_pos, 
            GLfloat ini_jogador_ang, 
            cor ini_jogador_corpo_cor, 
            cor ini_jogador_tiro_cor) :
        jogador_vida            {VIDAS_INICIAL},
        jogador_flag_andando    {0},
        jogador_tempo_andando   {0},
        jogador_pos             {ini_jogador_pos},
        jogador_dir             {},
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

    void desenha_corpo();
    void desenha_tiros();
    void move(GLfloat t_dif);
    void para();
    void gira_corpo(GLfloat t_dif);
    void gira_braco(GLfloat t_dif);
    void atira();
    bool verifica_colisao_inimigo(const Jogador3d& inimigo);
    bool verifica_colisao_obstaculos(const std::list<Obstaculo>& obstaculos);
    bool verifica_colisao_arena();
    
    void dano();

};




#endif // _JOGADOR_H_
