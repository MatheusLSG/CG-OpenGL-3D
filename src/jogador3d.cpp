#include "../lib/jogador3d.h"
#include "../lib/config.h" 
#include "../lib/obstaculo3d.h"
#include <GL/gl.h>

void Jogador3d::atualiza_animacao()
{
    
    int aux = 0;

    if (jogador_flag_anim)
    {
        aux = jogador_modelo.next();
    }
    else
    {
        return;
    }
    
    if (aux && jogador_anim_atual != PERDER)
    {
        jogador_modelo.drawInit(PARADO);  
    }
    
    if (aux && jogador_estado_atual == PERDER)
    {
        jogador_flag_anim = 0;
        return;
    }
    

    
    
    if (aux && jogador_estado_atual == PULANDO)
    {
        jogador_estado_atual = CAINDO;
        
    }
    else if (aux && jogador_anim_atual== ATACANDO)
    {
        jogador_estado_atual = PARADO;
        
    }
    else if (aux && jogador_anim_atual== DANO)
    {
        jogador_estado_atual = PARADO;
        
    }

    if (jogador_anim_atual != jogador_estado_atual || aux)
    {
        switch (jogador_estado_atual)
        {
        case PARADO:
            jogador_modelo.drawInit(PARADO);
            jogador_anim_atual = jogador_estado_atual;
            
            break;
            
        case ANDANDOFRENTE:
            jogador_modelo.drawInit(ANDANDOFRENTE);
            jogador_anim_atual = jogador_estado_atual;
            
            break;
            
        case ANDANDOTRAS:
            jogador_modelo.drawInit(ANDANDOTRAS);
            jogador_anim_atual = jogador_estado_atual;
            
            break;
        
        case CORRENDOFRENTE:
            jogador_modelo.drawInit(CORRENDOFRENTE);
            jogador_anim_atual = jogador_estado_atual;
            
            break;
        
        case CORRENDOTRAS:
            jogador_modelo.drawInit(CORRENDOTRAS);
            jogador_anim_atual = jogador_estado_atual;
            
            break;
            
        case PULANDO:
            jogador_modelo.drawInit(PULANDO);
            jogador_anim_atual = jogador_estado_atual;
            
            break;
            
        case CAINDO:
            jogador_modelo.drawInit(CAINDO);
            jogador_anim_atual = jogador_estado_atual;
            
            break;
            
        case ATACANDO:
            jogador_modelo.drawInit(ATACANDO);
            jogador_anim_atual = jogador_estado_atual;
            
            break;
            
        case DANO:
            jogador_modelo.drawInit(DANO);
            jogador_anim_atual = jogador_estado_atual;
            
            break;
            
        case GANHAR:
            jogador_modelo.drawInit(GANHAR);
            jogador_anim_atual = jogador_estado_atual;
            
            break;
            
        case PERDER:
            jogador_modelo.drawInit(PERDER);
            jogador_anim_atual = jogador_estado_atual;
            
            break;
            
        default:
            break;
        }
    }
}

void Jogador3d::desenha_jogador()
{
    

    glPushMatrix();

        glTranslatef(jogador_pos.x(), jogador_arma_pos.y(), jogador_pos.z());
        
        glRotatef(Jogador_theta, 0,1,0);
        
        jogador_modelo.drawCurrent();
    
    glPopMatrix();
}

void Jogador3d::desenha_tiros()
{

}

void Jogador3d::move(GLfloat s_dif)
{
    
    if (s_dif == 1)
    {
        jogador_estado_atual = ANDANDOFRENTE;
    }

    if (s_dif == 2)
    {
        jogador_estado_atual = CORRENDOFRENTE;
    }

    if (s_dif == -1)
    {
        jogador_estado_atual = ANDANDOTRAS;
    }

    if (s_dif == -2)
    {
        jogador_estado_atual = CORRENDOTRAS;
    }

    
}

void Jogador3d::para()
{
    jogador_estado_atual = PARADO;
}

void Jogador3d::pula()
{
    if (jogador_estado_atual != CAINDO)
    {
        jogador_estado_atual = PULANDO;
    }
   
}

void Jogador3d::gravidade(GLfloat t_dif)
{

}

void Jogador3d::gira_corpo(GLfloat theta_dif)
{

}

void Jogador3d::gira_arma(GLfloat theta_dif, GLfloat phi_dif)
{

}

void Jogador3d::atira()
{
    jogador_estado_atual = ATACANDO;
}

bool Jogador3d::verifica_colisao_inimigo(const Jogador3d& inimigo)
{

}

bool Jogador3d::verifica_colisao_obstaculos(const std::list<Obstaculo3d>& obstaculos)
{

}

bool Jogador3d::verifica_colisao_arena()
{

}


void Jogador3d::dano()
{
    if (jogador_vida > 0 && !jogador_inv)
    {
        jogador_vida--;
        
        if (jogador_vida)
        {
            jogador_estado_atual = DANO;
            jogador_modelo.drawInit(DANO);
        }
        else
            jogador_estado_atual = PERDER;
    }
}
