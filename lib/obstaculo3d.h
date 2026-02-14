#if !defined(_OBSTACULO_H_)
#define _OBSTACULO_H_


#include <GL/gl.h> 
#include "../lib/vec3.h"

class Obstaculo
{ 
    vec3 obstaculo_pos;
    GLfloat obstaculo_raio;
    cor  obstaculo_cor;
public: 
    Obstaculo(vec3 pos_ini, GLfloat raio, cor cor) :
        obstaculo_pos   {pos_ini},
        obstaculo_raio  {raio},
        obstaculo_cor   {cor}
    {}; 

    GLfloat raio() {return obstaculo_raio;};
    vec3 pos() {return obstaculo_pos;};
    
    void desenha();
};

 



#endif // _OBSTACULO_H_
