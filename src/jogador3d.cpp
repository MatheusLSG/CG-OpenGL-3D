#include "../lib/jogador3d.h"
#include "../lib/config.h"
#include "../lib/obstaculo3d.h"
#include <GL/gl.h>
#include <cmath>

/** Distância no plano XZ entre dois pontos. */
static GLfloat _dist_xz(const vec3& a, const vec3& b) {
    GLfloat dx = a.x() - b.x();
    GLfloat dz = a.z() - b.z();
    return std::sqrt(dx * dx + dz * dz);
}

/** Nível do chão em (x,z): 0 ou topo de pilastras/outro jogador sob o ponto. */
static GLfloat _get_ground_level(GLfloat x, GLfloat z, GLfloat raio_jogador,
                                 const std::list<Obstaculo3d>& obstaculos,
                                 const Jogador3d* outro) {
    GLfloat ground = 0.f;
    vec3 p(x, 0.f, z);
    for (const Obstaculo3d& o : obstaculos) {
        if (o.tipo() != OBSTACULO_REPULSOR) continue;
        if (_dist_xz(p, o.pos()) <= o.raio() + raio_jogador)
            ground = std::max(ground, o.pos().y() + o.altura());
    }
    if (outro && _dist_xz(p, outro->pos()) <= raio_jogador + outro->raio())
        ground = std::max(ground, outro->pos().y() + JOGADOR_ALTURA);
    return ground;
}

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
    
    if (aux && jogador_anim_atual != PERDER && jogador_anim_atual != PULANDO)
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
        jogador_flag_anim = 0;
        return;
    }

    
    
    /* PULANDO/CAINDO são controlados pela gravidade em gravidade(), não pela animação */
    if (aux && jogador_anim_atual== ATACANDO)
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

void Jogador3d::desenha_arma(){
    glPushMatrix();
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDisable(GL_ALPHA_TEST);

        glPushMatrix();
        
        GLfloat marrom[] = { 0.537, 0,317, 0,160, 1.0 };

        glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, marrom);
        glutSolidCube(10);

        glPopMatrix();

        glEnable(GL_ALPHA_TEST);
        glAlphaFunc(GL_GREATER, 0.5f);
        glDisable(GL_BLEND);

    glPopMatrix();
}

void Jogador3d::desenha_jogador()
{
    glPushMatrix();
        glCullFace(GL_BACK);

        glTranslatef(jogador_pos.x(), jogador_arma_pos.y(), jogador_pos.z());
        
        glRotatef(jogador_theta, 0,1,0);
        
        jogador_modelo.drawCurrent();

        glTranslatef(COORD_MAO_JOGADOR.x(), COORD_MAO_JOGADOR.y(), COORD_MAO_JOGADOR.z()); //Cordenadas do modelo
        glRotatef(jogador_arma_phi,1,0,0);
        glRotatef(jogador_arma_tetha,0,1,0);
        
        
        GLfloat marrom_difuso[] = { 0.45f, 0.25f, 0.14f, 1.0f };
        GLfloat preto[]         = { 0.0f, 0.0f, 0.0f, 1.0f };

        // Aplica ao material
        glMaterialfv(GL_FRONT, GL_DIFFUSE, marrom_difuso);
        glMaterialfv(GL_FRONT, GL_SPECULAR, preto); // Remove brilho
        glMaterialf(GL_FRONT, GL_SHININESS, 0.0f);   // Brilho zero
        glColor3f(0.45f, 0.25f, 0.14f);
        glTranslatef(0,0,7.5);
        glScalef(1, 1, 15);
        glutSolidCube(1);

    glPopMatrix();
}



void Jogador3d::desenha_tiros()
{
    for (auto tiro = tiros.begin(); tiro != tiros.end(); ++tiro) {
        (*tiro)->desenha();
    }
}

void Jogador3d::move(GLfloat s_dif)
{
    if (esta_no_ar()) {
        jogador_movimento_ar = (int)s_dif;  /* no ar: só guarda intenção para fazer arco (frente/trás) */
        return;
    }
    jogador_movimento_ar = 0;
    if (s_dif == 1)
        jogador_estado_atual = ANDANDOFRENTE;
    else if (s_dif == 2)
        jogador_estado_atual = CORRENDOFRENTE;
    else if (s_dif == -1)
        jogador_estado_atual = ANDANDOTRAS;
    else if (s_dif == -2)
        jogador_estado_atual = CORRENDOTRAS;
}

void Jogador3d::atualiza_movimento(GLdouble time_dif)
{
    GLfloat passo = (GLfloat)(JOGADOR_VELOCIDADE * (time_dif / 1000.0));
    vec3 desl(0, 0, 0);

    if (esta_no_ar() && jogador_movimento_ar != 0) {
        /* No ar: permite andar para frente/trás e fazer arco no pulo */
        if (jogador_movimento_ar > 0)
            desl = passo * jogador_dir;
        else
            desl = -passo * jogador_dir;
        jogador_pos += desl;
        jogador_arma_pos.e[0] = jogador_pos.x();
        jogador_arma_pos.e[2] = jogador_pos.z();
        return;
    }

    switch (jogador_estado_atual) {
        case ANDANDOFRENTE:
        case CORRENDOFRENTE:
            desl = passo * jogador_dir;
            break;
        case ANDANDOTRAS:
        case CORRENDOTRAS:
            desl = -passo * jogador_dir;
            break;
        default:
            return;
    }

    jogador_pos += desl;
    jogador_arma_pos.e[0] = jogador_pos.x();
    jogador_arma_pos.e[2] = jogador_pos.z();
}

void Jogador3d::para()
{
    if (esta_no_ar())
        jogador_movimento_ar = 0;
    else
        if(jogador_estado_atual != ATACANDO && jogador_estado_atual != DANO && jogador_estado_atual != PERDER && jogador_estado_atual != GANHAR) 
            jogador_estado_atual = PARADO;
}

void Jogador3d::pula()
{
    if (!esta_no_ar()) {
        jogador_y_inicio_pulo = jogador_pos.y();
        jogador_vy = VELOCIDADE_PULO_INICIAL;
        jogador_estado_atual = PULANDO;
    }
}

void Jogador3d::pula_soltar()
{
    if (jogador_estado_atual == PULANDO && jogador_vy > 0.f)
        jogador_vy = 0.f;
}

bool Jogador3d::esta_no_ar() const
{
    return jogador_estado_atual == PULANDO || jogador_estado_atual == CAINDO;
}

void Jogador3d::gravidade(GLfloat t_dif, const std::list<Obstaculo3d>& obstaculos, const Jogador3d* outro)
{
    GLfloat ground = _get_ground_level(jogador_pos.x(), jogador_pos.z(), jogador_raio, obstaculos, outro);

    if (!esta_no_ar()) {
        if (jogador_pos.y() > ground + 1e-3f) {
            jogador_estado_atual = CAINDO;
            jogador_modelo.drawInit(CAINDO);
            jogador_flag_anim = 1;
            jogador_vy = 0.f;
        } else {
            return;
        }
    }

    jogador_vy -= GRAVIDADE * (GLfloat)t_dif;
    jogador_pos.e[1] += jogador_vy * (GLfloat)t_dif;
    jogador_arma_pos.e[1] = jogador_pos.y();

    /* Limite do pulo: 2x a altura do personagem em relação ao chão de onde pulou */
    GLfloat pulo_teto_y = jogador_y_inicio_pulo + PULO_ALTURA_MAXIMA;
    if (jogador_pos.y() > pulo_teto_y) {
        jogador_pos.e[1] = pulo_teto_y;
        jogador_arma_pos.e[1] = pulo_teto_y;
        jogador_vy = 0.f;
        jogador_estado_atual = CAINDO;
        jogador_modelo.drawInit(CAINDO);
        jogador_flag_anim = 1;
    }

    /* Teto da arena: cabeça (y + JOGADOR_ALTURA) não pode passar do teto (4x altura do personagem) */
    GLfloat teto_max_y = TETO_ARENA_ALTURA - JOGADOR_ALTURA;
    if (jogador_pos.y() > teto_max_y) {
        jogador_pos.e[1] = teto_max_y;
        jogador_arma_pos.e[1] = teto_max_y;
        jogador_vy = 0.f;
        jogador_estado_atual = CAINDO;
        jogador_modelo.drawInit(CAINDO);
        jogador_flag_anim = 1;
    }

    if (jogador_vy <= 0.f && jogador_pos.y() <= ground + 1e-2f) {
        jogador_pos.e[1] = ground;
        jogador_arma_pos.e[1] = ground;
        jogador_vy = 0.f;
        if(jogador_estado_atual != ATACANDO)
            jogador_estado_atual = PARADO;
    } else if (jogador_vy < 0.f && jogador_estado_atual == PULANDO) {
        jogador_estado_atual = CAINDO;
        jogador_modelo.drawInit(CAINDO);
        jogador_flag_anim = 1;
    }
}

void Jogador3d::gira_corpo(GLfloat theta_dif)
{
    jogador_theta += theta_dif;
    jogador_dir = rotacao3Dy(jogador_theta, vec3(0, 0, 1));
}

void Jogador3d::set_pos_xz(GLfloat x, GLfloat z)
{
    jogador_pos.e[0] = x;
    jogador_pos.e[2] = z;
    jogador_arma_pos.e[0] = x;
    jogador_arma_pos.e[2] = z;
}

void Jogador3d::set_posicao_e_rotacao(vec3 pos, GLfloat theta_graus)
{
    jogador_pos.e[0] = pos.x();
    jogador_pos.e[1] = 0.f;
    jogador_pos.e[2] = pos.z();
    jogador_theta = theta_graus;
    jogador_dir = rotacao3Dy(jogador_theta, vec3(0, 0, 1));
    jogador_arma_pos = jogador_pos;
}

void Jogador3d::gira_arma(GLfloat theta_dif, GLfloat phi_dif)
{
    jogador_arma_tetha += theta_dif;
    if (jogador_arma_tetha > 45) jogador_arma_tetha = 45;
    if (jogador_arma_tetha < -45) jogador_arma_tetha = -45;

    jogador_arma_phi += phi_dif;
    if (jogador_arma_phi > 45) jogador_arma_phi = 45;
    if (jogador_arma_phi < -45) jogador_arma_phi = -45;
}

void Jogador3d::atira()
{
    if (jogador_estado_atual != CAINDO && jogador_estado_atual != PULANDO) 
        jogador_estado_atual = ATACANDO;

    // Base
    vec3 tiro_braco_base = vec3(COORD_MAO_JOGADOR);
    tiro_braco_base = rotacao3Dy(jogador_theta, tiro_braco_base);
    tiro_braco_base = translacao3D(jogador_pos.x(), jogador_pos.y(), jogador_pos.z(), tiro_braco_base);
    
    vec3 tiro_braco_topo = vec3(0,0,15);
    tiro_braco_topo = rotacao3Dy(jogador_arma_tetha, tiro_braco_topo);
    tiro_braco_topo = rotacao3Dx(jogador_arma_phi, tiro_braco_topo);
    tiro_braco_topo = translacao3D(COORD_MAO_JOGADOR.x(), COORD_MAO_JOGADOR.y(), COORD_MAO_JOGADOR.z(), tiro_braco_topo);
    
    tiro_braco_topo = rotacao3Dy(jogador_theta, tiro_braco_topo);
    tiro_braco_topo = translacao3D(jogador_pos.x(), jogador_pos.y(), jogador_pos.z(), tiro_braco_topo);
    
    // diff para dir
    vec3 tiro_dir = tiro_braco_topo - tiro_braco_base;
    tiro_dir = tiro_dir / tiro_dir.length();
    // adiciona
    tiros.push_back( 
                    new Tiro3d(
                    tiro_braco_topo, 
                    tiro_dir, 
                    jogador_tiro_raio, 
                    jogador_tiro_cor, 
                    this
                )
            );
}

bool Jogador3d::verifica_colisao_inimigo(const Jogador3d& inimigo) const
{
    GLfloat d = _dist_xz(jogador_pos, inimigo.pos());
    return d < jogador_raio + inimigo.raio();
}

bool Jogador3d::verifica_colisao_obstaculos(const std::list<Obstaculo3d>& obstaculos) const
{
    for (const Obstaculo3d& o : obstaculos) {
        if (o.tipo() != OBSTACULO_REPULSOR) continue;
        GLfloat d = _dist_xz(jogador_pos, o.pos());
        if (d < jogador_raio + o.raio())
            return true;
    }
    return false;
}

bool Jogador3d::verifica_colisao_arena(vec3 centro_arena, GLfloat raio_arena) const
{
    GLfloat d = _dist_xz(jogador_pos, centro_arena);
    return d + jogador_raio > raio_arena;
}

void Jogador3d::aplica_colisao_arena(vec3 centro_arena, GLfloat raio_arena)
{
    GLfloat d = _dist_xz(jogador_pos, centro_arena);
    if (d < 1e-6f) return;
    GLfloat limite = raio_arena - jogador_raio - COLISAO_MARGEM;
    if (limite <= 0.f) return;
    if (d > limite) {
        GLfloat f = limite / d;
        jogador_pos.e[0] = centro_arena.x() + (jogador_pos.x() - centro_arena.x()) * f;
        jogador_pos.e[2] = centro_arena.z() + (jogador_pos.z() - centro_arena.z()) * f;
        jogador_arma_pos.e[0] = jogador_pos.x();
        jogador_arma_pos.e[2] = jogador_pos.z();
    }
}

void Jogador3d::aplica_colisao_obstaculos(const std::list<Obstaculo3d>& obstaculos)
{
    const GLfloat eps_em_cima = 2.0f;  /* margem para considerar "em cima" do obstáculo */
    for (const Obstaculo3d& o : obstaculos) {
        if (o.tipo() != OBSTACULO_REPULSOR) continue;
        GLfloat d = _dist_xz(jogador_pos, o.pos());
        GLfloat soma_raios = jogador_raio + o.raio() + COLISAO_MARGEM;
        if (d < 1e-6f) continue;
        if (d < soma_raios) {
            /* Pilastra: se o jogador está em cima (pés >= topo da pilastra), permite ficar em cima */
            GLfloat topo_pilastra = o.pos().y() + o.altura();
            if (jogador_pos.y() >= topo_pilastra - eps_em_cima)
                continue;  /* não empurra em XZ; jogador pode ficar em cima da pilastra */
            GLfloat f = soma_raios / d;
            jogador_pos.e[0] = o.pos().x() + (jogador_pos.x() - o.pos().x()) * f;
            jogador_pos.e[2] = o.pos().z() + (jogador_pos.z() - o.pos().z()) * f;
            jogador_arma_pos.e[0] = jogador_pos.x();
            jogador_arma_pos.e[2] = jogador_pos.z();
        }
    }
}

void Jogador3d::aplica_colisao_inimigo(Jogador3d& outro)
{
    GLfloat d = _dist_xz(jogador_pos, outro.pos());
    GLfloat soma_raios = jogador_raio + outro.raio() + COLISAO_MARGEM;
    if (d < 1e-6f) return;
    if (d >= soma_raios) return;

    const GLfloat eps_em_cima = 2.0f;
    GLfloat meu_topo = jogador_pos.y() + JOGADOR_ALTURA;
    GLfloat topo_outro = outro.pos().y() + JOGADOR_ALTURA;
    /* Se um está em cima do outro, não empurra em XZ (permite subir em cima) */
    if (jogador_pos.y() >= topo_outro - eps_em_cima) return;  /* este está em cima do outro */
    if (outro.pos().y() >= meu_topo - eps_em_cima) return;   /* o outro está em cima deste */

    GLfloat overlap = soma_raios - d;
    GLfloat dx = (jogador_pos.x() - outro.pos().x()) / d;
    GLfloat dz = (jogador_pos.z() - outro.pos().z()) / d;
    GLfloat metade = overlap * 0.5f;
    jogador_pos.e[0] += dx * metade;
    jogador_pos.e[2] += dz * metade;
    jogador_arma_pos.e[0] = jogador_pos.x();
    jogador_arma_pos.e[2] = jogador_pos.z();
    outro.set_pos_xz(
        outro.pos().x() - dx * metade,
        outro.pos().z() - dz * metade
    );
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
