#include "../lib/jogador3d.h"
#include "../lib/config.h"
#include "../lib/obstaculo3d.h"
#include <GL/gl.h>
#include <cmath>

/** Altura do personagem mais alto; usada para teto da arena. Definida em main.cpp. */
extern GLfloat jogador_altura_global;

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
        ground = std::max(ground, outro->pos().y() + outro->altura());
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

void Jogador3d::desenha_arma(int flag_ligar_luz)
{
    GLfloat escala_altura = jogador_altura / (GLfloat)JOGADOR_ALTURA;
    /* Espessura da varinha em relação ao raio do personagem; fator < 1 deixa mais fina */
    GLfloat espessura_varinha = (jogador_raio / (GLfloat)JOGADOR_RAIO) * 0.65f;

    glPushMatrix();
        glTranslatef(jogador_pos.x(), jogador_pos.y(), jogador_pos.z());
        glRotatef(jogador_theta, 0,1,0);
        glTranslatef(escala_altura * jogador_arma_pos.x(), escala_altura * jogador_arma_pos.y(), escala_altura * jogador_arma_pos.z());
        glRotatef(jogador_arma_phi,1,0,0);
        glRotatef(jogador_arma_tetha,0,1,0);

        GLfloat marrom_difuso[] = { 0.45f, 0.25f, 0.14f, 1.0f };
        GLfloat preto[]         = { 0.0f, 0.0f, 0.0f, 1.0f };

        glMaterialfv(GL_FRONT, GL_DIFFUSE, marrom_difuso);
        glMaterialfv(GL_FRONT, GL_SPECULAR, preto);
        glMaterialf(GL_FRONT, GL_SHININESS, 0.0f);
        glColor3f(0.45f, 0.25f, 0.14f);
        glTranslatef(0,0, escala_altura * jogador_arma_tamanho);
        if (flag_ligar_luz)
        {
            glEnable(GL_LIGHTING);
            glEnable(jogador_luz);
            GLfloat zero[] = {0.0f, 0.0f, 0.0f, 1.0f};
            glLightfv(jogador_luz, GL_POSITION, zero);

            GLfloat direcao_foco[] = {jogador_arma_dir.x(), jogador_arma_dir.y(), jogador_arma_dir.z(), 0.0f};
            glLightfv(jogador_luz, GL_SPOT_DIRECTION, direcao_foco);
        }
        else
        {
            glDisable(jogador_luz);
        }
        glTranslatef(0,0, -escala_altura * jogador_arma_tamanho/2);
        glScalef(espessura_varinha, espessura_varinha, escala_altura * jogador_arma_tamanho);
        glutSolidCube(1);
    glPopMatrix();
}

void Jogador3d::desenha_jogador(int flag_ligar_luz)
{
    glPushMatrix();
        glCullFace(GL_BACK);

        glTranslatef(jogador_pos.x(), jogador_pos.y(), jogador_pos.z());
        glRotatef(jogador_theta, 0,1,0);

        /* Escala o modelo para a cabeça ficar limitada pela altura do personagem (modelo padrão = JOGADOR_ALTURA) */
        GLfloat escala_altura = jogador_altura / (GLfloat)JOGADOR_ALTURA;
        glScalef(escala_altura, escala_altura, escala_altura);

        jogador_modelo.drawCurrent();

    glPopMatrix();

    desenha_arma(flag_ligar_luz);
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
        /* Pulo em relação ao personagem mais alto: mesma altura máxima para todos */
        jogador_vy = 2.f * jogador_altura_global;
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

    jogador_vy -= jogador_altura_global * (GLfloat)t_dif;  /* gravidade em relação ao personagem mais alto */
    jogador_pos.e[1] += jogador_vy * (GLfloat)t_dif;

    /* Limite do pulo: 2× a altura do personagem mais alto (igual para todos) */
    GLfloat pulo_teto_y = jogador_y_inicio_pulo + 2.f * jogador_altura_global;
    if (jogador_pos.y() > pulo_teto_y) {
        jogador_pos.e[1] = pulo_teto_y;
        jogador_vy = 0.f;
        jogador_estado_atual = CAINDO;
        jogador_modelo.drawInit(CAINDO);
        jogador_flag_anim = 1;
    }

    /* Teto da arena: cabeça (y + jogador_altura) não pode passar do teto */
    GLfloat teto_max_y = OBSTACULO_ALTURA_MULTIPLICADOR * jogador_altura_global - jogador_altura;
    if (jogador_pos.y() > teto_max_y) {
        jogador_pos.e[1] = teto_max_y;
        jogador_vy = 0.f;
        jogador_estado_atual = CAINDO;
        jogador_modelo.drawInit(CAINDO);
        jogador_flag_anim = 1;
    }

    if (jogador_vy <= 0.f && jogador_pos.y() <= ground + 1e-2f) {
        jogador_pos.e[1] = ground;
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
}

void Jogador3d::set_posicao_e_rotacao(vec3 pos, GLfloat theta_graus)
{
    jogador_pos.e[0] = pos.x();
    jogador_pos.e[1] = 0.f;
    jogador_pos.e[2] = pos.z();
    jogador_theta = theta_graus;
    jogador_dir = rotacao3Dy(jogador_theta, vec3(0, 0, 1));

    tiros.clear();
}

void Jogador3d::gira_arma(GLfloat theta_dif, GLfloat phi_dif)
{
    jogador_arma_tetha += theta_dif;
    if (jogador_arma_tetha > 45) jogador_arma_tetha = 45;
    if (jogador_arma_tetha < -45) jogador_arma_tetha = -45;

    jogador_arma_phi += phi_dif;
    if (jogador_arma_phi > 45) jogador_arma_phi = 45;
    if (jogador_arma_phi < -45) jogador_arma_phi = -45;

    vec3 arma_base = vec3(jogador_arma_pos);
    arma_base = rotacao3Dy(jogador_theta, arma_base);
    arma_base = translacao3D(jogador_pos.x(), jogador_pos.y(), jogador_pos.z(), arma_base);
    
    vec3 arma_topo = vec3(0,0,jogador_arma_tamanho);
    arma_topo = rotacao3Dy(jogador_arma_tetha, arma_topo);
    arma_topo = rotacao3Dx(jogador_arma_phi, arma_topo);
    arma_topo = translacao3D(jogador_arma_pos.x(), jogador_arma_pos.y(), jogador_arma_pos.z(), arma_topo);
    
    arma_topo = rotacao3Dy(jogador_theta, arma_topo);
    arma_topo = translacao3D(jogador_pos.x(), jogador_pos.y(), jogador_pos.z(), arma_topo);
    
    // diff para dir
    vec3 aux = arma_topo - arma_base;
    jogador_arma_dir = aux / aux.length();
}

void Jogador3d::atira()
{
    if (jogador_estado_atual != CAINDO && jogador_estado_atual != PULANDO) 
        jogador_estado_atual = ATACANDO;

    GLfloat escala_altura = jogador_altura / (GLfloat)JOGADOR_ALTURA;
    vec3 arma_pos_esc(escala_altura * jogador_arma_pos.x(), escala_altura * jogador_arma_pos.y(), escala_altura * jogador_arma_pos.z());
    GLfloat arma_tam_esc = escala_altura * jogador_arma_tamanho;

    vec3 tiro_arma_topo = vec3(0, 0, arma_tam_esc);
    tiro_arma_topo = rotacao3Dy(jogador_arma_tetha, tiro_arma_topo);
    tiro_arma_topo = rotacao3Dx(jogador_arma_phi, tiro_arma_topo);
    tiro_arma_topo = translacao3D(arma_pos_esc.x(), arma_pos_esc.y(), arma_pos_esc.z(), tiro_arma_topo);
    tiro_arma_topo = rotacao3Dy(jogador_theta, tiro_arma_topo);
    tiro_arma_topo = translacao3D(jogador_pos.x(), jogador_pos.y(), jogador_pos.z(), tiro_arma_topo);

    tiros.push_back(
        new Tiro3d(tiro_arma_topo, jogador_arma_dir, jogador_tiro_raio, jogador_tiro_cor, this)
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
    GLfloat meu_topo = jogador_pos.y() + jogador_altura;
    GLfloat topo_outro = outro.pos().y() + outro.altura();
    /* Se um está em cima do outro, não empurra em XZ (permite subir em cima) */
    if (jogador_pos.y() >= topo_outro - eps_em_cima) return;  /* este está em cima do outro */
    if (outro.pos().y() >= meu_topo - eps_em_cima) return;   /* o outro está em cima deste */

    GLfloat overlap = soma_raios - d;
    GLfloat dx = (jogador_pos.x() - outro.pos().x()) / d;
    GLfloat dz = (jogador_pos.z() - outro.pos().z()) / d;
    GLfloat metade = overlap * 0.5f;
    jogador_pos.e[0] += dx * metade;
    jogador_pos.e[2] += dz * metade;
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


void Jogador3d::posiciona_camera_arma()
{
    GLfloat escala_altura = jogador_altura / (GLfloat)JOGADOR_ALTURA;
    vec3 arma_pos_esc = vec3(escala_altura * jogador_arma_pos.x(), escala_altura * jogador_arma_pos.y(), escala_altura * jogador_arma_pos.z());
    GLfloat arma_tam_esc = escala_altura * jogador_arma_tamanho;

    glMatrixMode (GL_PROJECTION);
        glLoadIdentity();
        gluPerspective (45, (GLfloat)VIEWPORT_LARGURA/(GLfloat)VIEWPORT_DOWN_ALTURA, 1, 1000);
    glMatrixMode(GL_MODELVIEW);

    vec3 arma_base = arma_pos_esc + vec3(0, 1.5f * escala_altura, 0);
    arma_base = rotacao3Dy(jogador_theta, arma_base);
    arma_base = translacao3D(jogador_pos.x(), jogador_pos.y(), jogador_pos.z(), arma_base);

    vec3 arma_topo = vec3(0, 0, arma_tam_esc) + vec3(0, 1.55f * escala_altura, 0);
    arma_topo = rotacao3Dy(jogador_arma_tetha, arma_topo);
    arma_topo = rotacao3Dx(jogador_arma_phi, arma_topo);
    arma_topo = translacao3D(arma_pos_esc.x(), arma_pos_esc.y(), arma_pos_esc.z(), arma_topo);
    arma_topo = rotacao3Dy(jogador_theta, arma_topo);
    arma_topo = translacao3D(jogador_pos.x(), jogador_pos.y(), jogador_pos.z(), arma_topo);

    vec3 dir_arma = (arma_topo - arma_base) / (arma_topo - arma_base).length();

    gluLookAt(arma_base.x(), arma_base.y(), arma_base.z(), arma_topo.x(), arma_topo.y(), arma_topo.z(), 0.f, 1.f, 0.f);
}

void Jogador3d::posiciona_camera_olho()
{   
    glMatrixMode (GL_PROJECTION);
        glLoadIdentity();
        gluPerspective (75, (GLfloat)VIEWPORT_LARGURA/(GLfloat)VIEWPORT_UP_ALTURA, 1, 1000);
    glMatrixMode(GL_MODELVIEW);

    vec3 o = vec3(0,0,0);
    o = translacao3D(0, jogador_altura*0.9f, jogador_raio*0.9f, o);
    o = rotacao3Dy(jogador_theta, o);
    o = translacao3D(jogador_pos.x(), jogador_pos.y(), jogador_pos.z(), o);

    vec3 f = vec3(0,0,0);
    f = translacao3D(0, jogador_altura*0.9f, jogador_raio, f);
    f = rotacao3Dy(jogador_theta, f);
    f = translacao3D(jogador_pos.x(), jogador_pos.y(), jogador_pos.z(), f);

    gluLookAt(o.x(), o.y(), o.z(), f.x(), f.y(), f.z(), 0.f, 1.f, 0.f);
}