#include "../lib/tiro3d.h"
#include "../lib/config.h" 
#include "../lib/transform3d.h"
#include <GL/gl.h>

/** Distância no plano XZ entre dois pontos. */
static GLfloat _dist_xz(const vec3& a, const vec3& b) {
    GLfloat dx = a.x() - b.x();
    GLfloat dz = a.z() - b.z();
    return std::sqrt(dx * dx + dz * dz);
}

void Tiro3d::desenha()
{
    glPushMatrix();
        glCullFace(GL_BACK);
        // Posicao
        glTranslatef(tiro_pos_atual.x(), tiro_pos_atual.y(), tiro_pos_atual.z());
        
        
        GLfloat emiss[4] = { tiro_cor.x()*((GLfloat)0.8), tiro_cor.y()*((GLfloat)0.8), tiro_cor.z()*((GLfloat)0.8), 1.f };
        GLfloat amb[4]   = { tiro_cor.x()*((GLfloat)0.2), tiro_cor.y()*((GLfloat)0.2), tiro_cor.z()*((GLfloat)0.2), 1.f };
        glMaterialfv(GL_FRONT, GL_EMISSION, emiss);
        glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, amb);
        glMaterialfv(GL_FRONT, GL_SPECULAR, emiss);
        glColor3f(tiro_cor.x(), tiro_cor.y(), tiro_cor.z());
        glutSolidSphere(tiro_raio, 16, 16);
        GLfloat zero[4] = { 0.f, 0.f, 0.f, 1.f };
        glMaterialfv(GL_FRONT, GL_EMISSION, zero);
     
    glPopMatrix();
}

void Tiro3d::move(GLfloat dist)
{
    
    tiro_pos_atual += tiro_dir*dist;
}


bool Tiro3d::valido(GLfloat t_dif)
{
    tiro_tempo_atual += t_dif;
    return tiro_tempo_atual < tiro_tempo_limite;  
}

bool Tiro3d::verifica_colisao_jogador(Jogador3d& jogador)
{
    bool colidiu = false;

    GLfloat j_raio = jogador.raio();
    GLfloat j_altura = jogador.altura();
    vec3 j_pos = jogador.pos();

    GLfloat t_raio = this->tiro_raio;
    vec3    t_pos = this->tiro_pos_atual;

    GLdouble soma_raios = j_raio +t_raio;
    vec3 pos_diff = (vec3(t_pos.x(), 0, t_pos.z()) - vec3(j_pos.x(), j_pos.y(),  j_pos.z()));
    
    if ( (GLdouble) soma_raios*soma_raios > (GLdouble) pos_diff.length_squared())
    {
        GLfloat j_y = j_pos.y();
        GLfloat t_y = t_pos.y();
        
        if (t_y - t_raio < j_y+j_altura && t_y + t_raio > j_y)
        {
            
            
            jogador.dano();
            
            colidiu = true;
        }
    }
    
    return colidiu;
}

bool Tiro3d::verifica_colisao_obstaculos(const std::list<Obstaculo3d>& obstaculos)
{
    bool colidiu = false;

    GLfloat o_raio =0;
    GLfloat o_altura =0;
    vec3 o_pos;
    GLfloat o_y =0;

    GLfloat t_raio = this->tiro_raio;
    vec3 t_pos = this->tiro_pos_atual;
    GLfloat t_y = t_pos.y();
    
    for (Obstaculo3d o : obstaculos)
    {
        o_raio = o.raio();
        o_pos = o.pos();   
        o_altura = o.altura();

        if (o.tipo() == OBSTACULO_REPULSOR)
        {
            GLfloat d = _dist_xz(t_pos, o_pos);
            GLdouble soma_raios = o_raio +t_raio;
            if (d < 1e-6f) continue;
            if (d < soma_raios) {
                o_y = o_pos.y();
         
                if (t_y - t_raio < o_y+o_altura)
                {
                    //cout << "Bateu na Pilastra\n";
                    return true;
                }
            }    
        }
        else if (o.tipo() == OBSTACULO_ATRATOR)
        {
            GLdouble diff_raios = o_raio - t_raio;
            GLfloat d = _dist_xz(t_pos, o_pos);
            o_y = o_pos.y();
            
            if ( d > diff_raios)
            {
                //cout << "Bateu na parede da Arena\n";
                return true;    
            } 
            else if (t_y - t_raio < o_y || t_y + t_raio > o_y+o_altura)
            {
                //cout << "Bateu no teto da Arena\n";
                return true;
            } 
        }
    }
    
    return false;
}


