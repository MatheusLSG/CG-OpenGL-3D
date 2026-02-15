#if !defined(_TIRO_H_)
#define _TIRO_H_


#include <GL/gl.h>
#include <GL/glu.h>
#include "../lib/vec3.h"
#include "../lib/jogador3d.h"
#include "../lib/transform3d.h"
#include "../lib/obstaculo3d.h"
#include "../lib/config.h"

// momento confia
class Jogador3d;

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
    vec3 pos() const {return tiro_pos_atual;};

    void desenha();

    void move(GLfloat dist);
    
    bool valido(GLfloat t_dif);
    
    bool verifica_colisao_jogador(Jogador3d& jogador);
    bool verifica_colisao_obstaculos(const std::list<Obstaculo3d>& obstaculos);
    bool verifica_colisao_arena();
};

#endif // _TIRO_H_
