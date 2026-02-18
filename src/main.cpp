#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <math.h>
#include <cstring>
#include <stdlib.h>
#include <list>
#include <algorithm>

#include "../lib/tinyxml2.h"
using namespace tinyxml2;

#include "../lib/objloader.h"
#include "../lib/main.h"

#include "../lib/tiro3d.h"
#include "../lib/jogador3d.h"
#include "../lib/obstaculo3d.h"
#include "../lib/placar.h"
#include "../lib/ControladorJogo.h"
#include "../lib/leitorDeSVG.h"
#include "../lib/config.h"
#include "../lib/pomoDeOuro.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// VARIAVEIS GLOBAIS



// placar

Placar placar;

// arena (path do SVG passado por linha de comando)
char* arquivo_arena = nullptr;

// Dados carregados do SVG (se arquivo foi passado). Mantém proporções.
DadosArenaSVG dadosArenaSVG;

// Altura do personagem mais alto. Usada por funções gerais: arena, obstáculos, pomo de ouro (não pelo pulo/colisão pessoal).
GLfloat jogador_altura_global = JOGADOR_ALTURA;

// teclas
int teclas[512];

/** Pulo: só permite novo pulo após soltar a tecla (máquina de estados). */
static bool pulo_tecla_liberada_harry = true;
static bool pulo_tecla_liberada_draco = true;

// mouse
GLfloat mouse_x_diff = 0;
GLfloat mouse_y_diff = 0;

int mouse_esquerda = 0;

// dimensao da janela
const GLint janela_largura = JANELA_LARGURA;
const GLint janela_altura = JANELA_ALTURA;

// texto
static char str[2000];
void * fonte = TEXTO_FONTE;

// jogadores

Jogador3d jHarry;
Jogador3d jDraco;

// obstaculos

std::list<Obstaculo3d> obstaculos;

// Texturas geradas em código (sem arquivos em resources/arena)
GLuint tex_parede_id = 0;

// flags

bool flag_col_tiros = PERMITE_COLISAO_ENTRE_TIROS;
int flag_ganhador = 0;
int flag_perspectiva = 1;
int flag_lanterna = 0;

// OLD:

//Malhas a serem desenhadas
meshes harry;
int harryIdle = -1;




//Controle do tempo
//Soh trata maquinas mais rapidas, nao faz animacao relativa ao tempo
int currentTime = 0;
int lastTime = 0;
int updateDrawing = 0;
// Delta de tempo (ms) para movimento independente da taxa de quadros
static GLdouble previousTime = 0;

//Controles gerais
int zoom = 150;
int lookatToggle = 0;
int transformacaoArmaToggle = 0;
int armaToggle = 0;
int coordsysToggle = 0;
double camXYAngle=0;
double camXZAngle=0;
int lastX = 0;
int lastY = 0;
int mouse_direita=0;
int flag_swap = 1;


// Pomo de Ouro: única luz do jogo (toggle com tecla N)
PomoDeOuro pomoDeOuro;
bool forcar_zero_emissao = false;      // usado por objloader para zerar emissão quando luz off

void inicializacao();
void renderiza_cena();
void idle();

void reinicia_teclas();
void reinicia_flags();

void pressionar_teclas(unsigned char tecla, int x, int y);
void soltar_teclas(unsigned char tecla, int x, int y);
void keyUp(unsigned char key, int x, int y);

void mouse_press(int botao, int estado, int x, int y);
void mouse_move(int x, int y);

void coleta_dados_arena();
void desenhaChaoAzul();

GLuint geraTexturaDifusaParedes();

void idle_jogo(int timeDiference);
void idle_tiros(std::list<Tiro3d*>& tiros_aliado, Jogador3d& inimigo, GLdouble t_dif);

void renderiza_texto_direita(vec3 pos, cor cor_texto, char *texto);
void renderiza_texto_esquerda(vec3 pos, cor cor_texto, char *texto);
void renderiza_texto_centro(vec3 pos, cor cor_texto, char *texto);


void camera_terceira_pessoa_draco();
void camera_terceira_pessoa_harry();

void configura_lanterna(int luz, GLfloat luz_difusa[4], GLfloat luz_especular[4] );

void mouse(int button, int state, int x, int y);
void mouse_motion(int x, int y);
void display();
void keyPress(unsigned char key, int x, int y);

// MAIN --------------------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    if (argc >= 2) {
        arquivo_arena = argv[1];
        if (!carregaArenaSVG(argv[1], (float)ARENA_RAIO_PADRAO, &dadosArenaSVG)) {
            if (!carregaArenaSVG("arena/arena.svg", (float)ARENA_RAIO_PADRAO, &dadosArenaSVG)) {
                if (!carregaArenaSVG("../arena/arena.svg", (float)ARENA_RAIO_PADRAO, &dadosArenaSVG)) {
                    std::cerr << "Erro ao carregar SVG: " << argv[1] << "\n";
                    return 1;
                }
            }
        }
    }

    // glut
    glutInit(&argc, argv);
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    
    // Cria a janela
    glutInitWindowSize(janela_largura, janela_altura);
    glutInitWindowPosition(150,50);
    glutCreateWindow ("Jogo 3D");
 
    // Alocacao das callbacks
    /*
    glutDisplayFunc(renderiza_cena);
    glutKeyboardFunc(pressionar_teclas);
    glutIdleFunc(idle);
    glutKeyboardUpFunc(soltar_teclas);
    glutMouseFunc(mouse_press);
    glutPassiveMotionFunc(mouse_move);
    glutMotionFunc(mouse_move);
    */

    glutIdleFunc(idle);
    glutDisplayFunc(display);
    glutMotionFunc(mouse_motion);
    glutPassiveMotionFunc(mouse_motion);
    glutMouseFunc(mouse);
    glutKeyboardFunc(keyPress);
    glutKeyboardUpFunc(keyUp);
    glutIgnoreKeyRepeat(1);  /* evita key repeat: segurar x/. não gera vários keyPress nem keyUp falso */

    // inicializa cena
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); 
    
    glShadeModel (GL_SMOOTH);
    glEnable(GL_LIGHTING);
    // Sem luz ambiente global: quando o Pomo de Ouro estiver desligado (N), a cena fica totalmente escura
    GLfloat zero[4] = { 0.f, 0.f, 0.f, 1.f }; 
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, zero);
    
    pomoDeOuro.configurarLuzOpenGL();
    glEnable(GL_DEPTH_TEST);

    glMatrixMode (GL_PROJECTION);
        glLoadIdentity();
        gluPerspective (45, (GLfloat)1/(GLfloat)1, 1, 1000);
    glMatrixMode(GL_MODELVIEW);

    glLoadIdentity();

    // Inicializa jogo
    inicializacao();

    // Loop principal
    glutMainLoop();
    
    return 0;
}

void inicializacao ()
{
    meshes draco;
    
    draco.loadMeshAnim("resources/draco/idle/idle####.obj", 192, 1);
    draco.loadMeshAnim("resources/draco/walkforw/walkforw####.obj", 34, 1);
    draco.loadMeshAnim("resources/draco/walkback/walkback####.obj", 36, 1);
    draco.loadMeshAnim("resources/draco/runforw/runforw####.obj", 21, 1);
    draco.loadMeshAnim("resources/draco/runback/runback####.obj", 21, 1);
    draco.loadMeshAnim("resources/draco/jump/jump####.obj", 26, 1);
    draco.loadMeshAnim("resources/draco/fall/fall####.obj", 33, 1);
    draco.loadMeshAnim("resources/draco/atack/atack####.obj", 74, 1);
    draco.loadMeshAnim("resources/draco/damage/damage####.obj", 31, 1);
    draco.loadMeshAnim("resources/draco/win/win####.obj", 374, 1);
    draco.loadMeshAnim("resources/draco/lose/lose####.obj", 96, 1);
    draco.drawInit(PARADO);
    
     
    vector<string> dracoTexturesPaths;
    dracoTexturesPaths.push_back("resources/draco/texture/dracoTex0.bmp");
    dracoTexturesPaths.push_back("resources/draco/texture/dracoTex1.bmp");
    dracoTexturesPaths.push_back("resources/draco/texture/dracoTex2.bmp");
    
    vector<int> dracoTransparente = {0,0,0};

    // if( !draco.loadTexture(dracoTexturesPaths, dracoTransparente) ) exit(printf("Lista de texturas invalidas!\n"));
    if( !draco.loadTexture(dracoTexturesPaths, dracoTransparente) ) { std::cerr << "Lista de texturas draco invalidas\n"; exit(1); }

    vec3 posDraco(0.f, 0.f, 0.f);
    GLfloat raio_draco = (GLfloat)JOGADOR_RAIO;
    if (dadosArenaSVG.ok) {
        posDraco = vec3(dadosArenaSVG.draco_x, 0.f, dadosArenaSVG.draco_z);
        raio_draco = (GLfloat)dadosArenaSVG.draco_r;
    }
    GLfloat alt_draco = 3.f * raio_draco;

    POS maoDraco = draco.vecMeshes[0][0].vertsPos[490];

    GLfloat raio_tiro_draco = (GLfloat)RAIO_TIRO * (alt_draco / (GLfloat)JOGADOR_ALTURA);
    jDraco = Jogador3d(
        raio_draco,
        alt_draco,
        0,
        posDraco,
        vec3(maoDraco.x, maoDraco.y, maoDraco.z), 
        VERDE,
        raio_tiro_draco,
        GL_LIGHT2,
        draco
    );

    GLfloat verde[] = {0.5f, 1.0f, 0.5f, 1.0f};
    configura_lanterna(GL_LIGHT2, verde, verde);

    // HARRY ----------------------------------------------------------------
    meshes harry;
    
    harry.loadMeshAnim("resources/harry/idle/idle####.obj", 192, 1);
    harry.loadMeshAnim("resources/harry/walkforw/walkforw####.obj", 34, 1);
    harry.loadMeshAnim("resources/harry/walkback/walkback####.obj", 36, 1);
    harry.loadMeshAnim("resources/harry/runforw/runforw####.obj", 21, 1);
    harry.loadMeshAnim("resources/harry/runback/runback####.obj", 21, 1);
    harry.loadMeshAnim("resources/harry/jump/jump####.obj", 26, 1);
    harry.loadMeshAnim("resources/harry/fall/fall####.obj", 33, 1);
    harry.loadMeshAnim("resources/harry/atack/atack####.obj", 74, 1);
    harry.loadMeshAnim("resources/harry/damage/damage####.obj", 31, 1);
    harry.loadMeshAnim("resources/harry/win/win####.obj", 374, 1);
    harry.loadMeshAnim("resources/harry/lose/lose####.obj", 96, 1);
    harry.drawInit(PARADO);
     
    
    vector<string> harryTexturesPaths;
    harryTexturesPaths.push_back("resources/harry/texture/harryTex0.bmp");
    harryTexturesPaths.push_back("resources/harry/texture/harryTex1.bmp");
    harryTexturesPaths.push_back("resources/harry/texture/harryTex2.bmp");
    harryTexturesPaths.push_back("resources/harry/texture/harryTex3.bmp");
    
    vector<int> harryTransparente = {0,0,1,0};

    // if( !harry.loadTexture(harryTexturesPaths, harryTransparente)) exit(printf("Lista de texturas invalidas!\n"));
    if( !harry.loadTexture(harryTexturesPaths, harryTransparente)) { std::cerr << "Lista de texturas harry invalidas\n"; exit(1); }

    vec3 posHarry(0.f, 0.f, 200.f);
    GLfloat raio_harry = (GLfloat)JOGADOR_RAIO;
    if (dadosArenaSVG.ok) {
        posHarry = vec3(dadosArenaSVG.harry_x, 0.f, dadosArenaSVG.harry_z);
        raio_harry = (GLfloat)dadosArenaSVG.harry_r;
    }
    GLfloat alt_harry = 3.f * raio_harry;

    POS maoHarry = harry.vecMeshes[0][0].vertsPos[665];

    GLfloat raio_tiro_harry = (GLfloat)RAIO_TIRO * (alt_harry / (GLfloat)JOGADOR_ALTURA);
    jHarry = Jogador3d(
        raio_harry,
        alt_harry,
        180,
        posHarry,
        vec3(maoHarry.x, maoHarry.y, maoHarry.z),
        VERMELHO,
        raio_tiro_harry,
        GL_LIGHT1,
        harry
    );
    
    GLfloat vermelho[] = {1.0f, 0.5f, 0.5f, 1.0f};
    configura_lanterna(GL_LIGHT1, vermelho, vermelho);

    // Altura de referência para funções gerais (arena, obstáculos, pomo de ouro): personagem mais alto
    jogador_altura_global = std::max(alt_draco, alt_harry);

    // ARENA -----------------------------
    if (dadosArenaSVG.ok) {
        obstaculos.clear();
        GLfloat alt = OBSTACULO_ALTURA_MULTIPLICADOR * jogador_altura_global;
        obstaculos.push_back(Obstaculo3d(
            vec3(dadosArenaSVG.arena_cx, 0.f, dadosArenaSVG.arena_cy),
            dadosArenaSVG.arena_r,
            AZUL,
            alt,
            OBSTACULO_ATRATOR,
            true   // teto da arena (sem sol no infinito, arena pode ser fechada)
        ));
        for (size_t i = 0; i < dadosArenaSVG.pilastras.size(); ++i) {
            const Circulo2D& p = dadosArenaSVG.pilastras[i];
            obstaculos.push_back(Obstaculo3d(
                vec3(p.cx, 0.f, p.cy),
                p.r,
                PRETO,
                jogador_altura_global,   // pilastras com altura do personagem (1x)
                OBSTACULO_REPULSOR
            ));
        }

        tex_parede_id = geraTexturaDifusaParedes();
        if (tex_parede_id) Obstaculo3d::setTexturaParedes(tex_parede_id);
    }
}

GLuint geraTexturaDifusaParedes()
{
    const int TAM = 64;
    unsigned char* pix = new unsigned char[TAM * TAM * 3];
    for (int y = 0; y < TAM; y++) {
        for (int x = 0; x < TAM; x++) {
            int v = 12 + ((x * 7 + y * 13) % 17);
            if (v > 28) v = 28;
            pix[(y * TAM + x) * 3 + 0] = (unsigned char)v;
            pix[(y * TAM + x) * 3 + 1] = (unsigned char)v;
            pix[(y * TAM + x) * 3 + 2] = (unsigned char)v;
        }
    }
    GLuint id = 0;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, TAM, TAM, 0, GL_RGB, GL_UNSIGNED_BYTE, pix);
    delete[] pix;
    return id;
}

// Desenha um disco no plano XY (z=0) com normal (0,0,1).
static void desenhaDisco(GLfloat raio, int numFatias)
{
    glCullFace(GL_BACK);
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0.0f, 0.0f, 1.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    for (int i = 0; i <= numFatias; ++i) {
        GLfloat t = (GLfloat)i / (GLfloat)numFatias * 2.0f * (GLfloat)M_PI;
        glVertex3f(raio * cosf(t), raio * sinf(t), 0.0f);
    }
    glEnd();
}

// Desenha a superfície lateral de um cilindro ao longo do eixo Z (z=0 até z=altura).
static void desenhaCilindroLateral(GLfloat raio, GLfloat altura, int numFatias)
{
    glCullFace(GL_BACK);
    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= numFatias; ++i) {
        GLfloat ang = (GLfloat)i / (GLfloat)numFatias * 2.0f * (GLfloat)M_PI;
        GLfloat cx = cosf(ang), sy = sinf(ang);
        glNormal3f(cx, sy, 0.0f);
        glVertex3f(raio * cx, raio * sy, 0.0f);
        glVertex3f(raio * cx, raio * sy, altura);
    }
    glEnd();
}

// Chão azul só com specular (tipo vidro), sem textura difusa.
void desenhaChaoAzul()
{
    if (!dadosArenaSVG.ok) return;

    glPushMatrix();
    glTranslatef(dadosArenaSVG.arena_cx, 0.f, dadosArenaSVG.arena_cy);
    glRotatef(-90.f, 1.f, 0.f, 0.f);

    GLfloat mat_amb_diff[4] = { AZUL.x(), AZUL.y(), AZUL.z(), 1.0f };
    GLfloat mat_spec[4]    = { 0.5f, 0.5f, 0.65f, 1.0f };
    GLfloat shininess[]    = { 90.0f };
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_amb_diff);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_spec);
    glMaterialfv(GL_FRONT, GL_SHININESS, shininess);
    glColor3f(AZUL.x(), AZUL.y(), AZUL.z());

    desenhaDisco(dadosArenaSVG.arena_r, PONTOS_POR_ELIPSE);

    glPopMatrix();
}

void DrawAxes(double size)
{
    GLfloat mat_ambient_r[] = { 1.0, 0.0, 0.0, 1.0 };
    GLfloat mat_ambient_g[] = { 0.0, 1.0, 0.0, 1.0 };
    GLfloat mat_ambient_b[] = { 0.0, 0.0, 1.0, 1.0 };
    GLfloat no_mat[] = { 0.0, 0.0, 0.0, 1.0 };
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, 
            no_mat);
    glMaterialfv(GL_FRONT, GL_SPECULAR, no_mat);
    glMaterialfv(GL_FRONT, GL_SHININESS, no_mat);

    glPushMatrix();
        glMaterialfv(GL_FRONT, GL_EMISSION, mat_ambient_r);
        glColor3fv(mat_ambient_r);
        glScalef(size, size*0.1, size*0.1);
        glTranslatef(0.5, 0, 0);
        glutSolidCube(1.0);
    glPopMatrix();

    glPushMatrix();
        glMaterialfv(GL_FRONT, GL_EMISSION, mat_ambient_g);
        glColor3fv(mat_ambient_g);
        glRotatef(90,0,0,1);
        glScalef(size, size*0.1, size*0.1);
        glTranslatef(0.5, 0, 0);
        glutSolidCube(1.0);
    glPopMatrix();

    glPushMatrix();
        glMaterialfv(GL_FRONT, GL_EMISSION, mat_ambient_b);
        glColor3fv(mat_ambient_b);
        glRotatef(-90,0,1,0);
        glScalef(size, size*0.1, size*0.1);
        glTranslatef(0.5, 0, 0);
        glutSolidCube(1.0);
    glPopMatrix();
}

void desenhaMundo(int flag_harry, int flag_draco){
    pomoDeOuro.aplicarLuz();

    desenhaChaoAzul();
    for (Obstaculo3d& o : obstaculos)
        o.desenha();
    if (pomoDeOuro.estaAtivo())
        pomoDeOuro.desenhar();
    
    
    glEnable(GL_ALPHA_TEST);
        glAlphaFunc(GL_GREATER, 0.5f);
        if(flag_harry) jHarry.desenha_jogador(flag_lanterna);
        if(flag_draco) jDraco.desenha_jogador(flag_lanterna);
        jHarry.desenha_tiros();
        jDraco.desenha_tiros();

    if (coordsysToggle == 1)  DrawAxes(83);
}

void desenhaJogo(){
    

    glMatrixMode(GL_MODELVIEW);
    glEnable(GL_CULL_FACE);
    glPushMatrix();
        // Translada para o centro do personagem para facilitar a rotacao da camera
        glTranslatef(0,-40,0);
        pomoDeOuro.atualizar(&dadosArenaSVG, OBSTACULO_ALTURA_MULTIPLICADOR * jogador_altura_global);
        forcar_zero_emissao = !pomoDeOuro.estaAtivo();
        if (!pomoDeOuro.estaAtivo()) {
            //GLfloat zero[4] = { 0.f, 0.f, 0.f, 1.f };
            //glLightModelfv(GL_LIGHT_MODEL_AMBIENT, zero);
            //for (int i = 0; i < 8; i++) glDisable((GLenum)(GL_LIGHT0 + i));
        }

        glLoadIdentity();

        // Viewport Harry: câmera acompanha Harry (olhar fixo no personagem, sem girar com ele)
        glViewport(0, 0, VIEWPORT_LARGURA, VIEWPORT_DOWN_ALTURA);
        glPushMatrix();
            if (flag_perspectiva)
            {
                camera_terceira_pessoa_harry();
                desenhaMundo(1,1);        
            }
            else
            {
                jHarry.posiciona_camera_arma();
                desenhaMundo(0,1);
                jHarry.desenha_arma(flag_lanterna);
            }


        glPopMatrix();

        // Viewport Draco: câmera acompanha Draco (olhar fixo no personagem, sem girar com ele)
        glViewport(VIEWPORT_LARGURA, 0, VIEWPORT_LARGURA, VIEWPORT_DOWN_ALTURA);
        glPushMatrix();
            if (flag_perspectiva)
            {
                camera_terceira_pessoa_draco();   
                desenhaMundo(1,1);     
            }
            else
            {
                jDraco.posiciona_camera_arma();
                desenhaMundo(1,0);
                jDraco.desenha_arma(flag_lanterna);
            }


        glPopMatrix();

        // Viewport Olho Harry: câmera acompanha Harry (olhar fixo no personagem, sem girar com ele)
        glViewport(0, VIEWPORT_DOWN_ALTURA, VIEWPORT_LARGURA, VIEWPORT_UP_ALTURA);
        glPushMatrix();
            jHarry.posiciona_camera_olho();

            desenhaMundo(1,1);


        glPopMatrix();

        // Viewport Olho Draco: câmera acompanha Draco (olhar fixo no personagem, sem girar com ele)
        glViewport(VIEWPORT_LARGURA, VIEWPORT_DOWN_ALTURA, VIEWPORT_LARGURA, VIEWPORT_UP_ALTURA);
        glPushMatrix();
            jDraco.posiciona_camera_olho();

            desenhaMundo(1,1);

        glPopMatrix();

    glPopMatrix();
}

// Retorna o controlador de jogo (singleton deste arquivo)
ControladorJogo& getControladorJogo()
{
    static ControladorJogo controladorJogo(jHarry, jDraco, placar);
    return controladorJogo;
}

void display(void)
{
    // Sempre desenha e troca buffers (evita tela vazia em hardware lento/WSL onde
    // muitos display() podem rodar antes do primeiro idle() setar updateDrawing).
    if (updateDrawing) {
        updateDrawing = 0;
        jHarry.atualiza_animacao();
        jDraco.atualiza_animacao();
        ControladorJogo& cj = getControladorJogo();
        cj.atualizar(0.0);
    }

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glClearColor (0.30, 0.30, 1.0, 0.0);
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    

    desenhaJogo();

    getControladorJogo().desenharHUD();

    glutSwapBuffers ();
}

void keyUp(unsigned char key, int x, int y)
{
    if ((unsigned int)key < 512)
        teclas[key] = 0;
    if ((unsigned int)key == 0xE7)
        teclas[0xE7] = 0;
    if ((unsigned int)key == 0xC7)  /* Ç maiúsculo */
        teclas[0xC7] = 0;
    if (key == 'x') {
        jHarry.pula_soltar();
        pulo_tecla_liberada_harry = true;  /* só libera para próximo pulo quando realmente soltou */
    }
    if (key == '.') {
        jDraco.pula_soltar();
        pulo_tecla_liberada_draco = true;
    }
    glutPostRedisplay();
}

void keyPress(unsigned char key, int x, int y)
{
    /* Pulo: só se a tecla foi liberada antes (soltar e apertar de novo) e está no chão */
    if (key == 'x' && pulo_tecla_liberada_harry && !jHarry.esta_no_ar()) {
        jHarry.pula();
        pulo_tecla_liberada_harry = false;  /* segurando: não dar outro pulo até keyUp */
    }
    if (key == '.' && pulo_tecla_liberada_draco && !jDraco.esta_no_ar()) {
        jDraco.pula();
        pulo_tecla_liberada_draco = false;
    }

    if ((unsigned int)key < 512)
        teclas[key] = 1;

    ControladorJogo& controladorJogo = getControladorJogo();

    switch(key){
    /*
    case '1':
        jDraco.para();
        jHarry.para();
        break;
    case '2':
        jDraco.move(1);
        jHarry.move(1);
        break;
    case '3':
        jDraco.move(2);
        jHarry.move(2);
        break;
    case '4':
        jDraco.move(-1);
        jHarry.move(-1);
        break;
    case '5':
        jDraco.move(-2);
        jHarry.move(-2);
        break;
    case '6':
        jDraco.pula();
        jHarry.pula();
        break;
    case '7':
        jDraco.atira();
        jHarry.atira();
        break;
    */
    case 'n':
    case 'N':
        pomoDeOuro.alternarAtivo();    
        flag_lanterna = 1;
        break;
    case 'c':
        coordsysToggle = !coordsysToggle;
        break;
    case 'm':
        lookatToggle = !lookatToggle;
        break;
    case '+':
        zoom++;
        break;
    case '-':
        zoom--;
         break;
    case 'r':
    case 'R': {
        GLfloat hx, hz, dx, dz;
        if (dadosArenaSVG.ok) {
            hx = (GLfloat)dadosArenaSVG.harry_x;
            hz = (GLfloat)dadosArenaSVG.harry_z;
            dx = (GLfloat)dadosArenaSVG.draco_x;
            dz = (GLfloat)dadosArenaSVG.draco_z;
        } else {
            hx = 0.f; hz = 200.f;
            dx = 0.f; dz = 0.f;
        }
        GLfloat theta_harry = (GLfloat)(atan2((double)(dx - hx), (double)(dz - hz)) * 180.0 / M_PI);
        GLfloat theta_draco = (GLfloat)(atan2((double)(hx - dx), (double)(hz - dz)) * 180.0 / M_PI);
        jHarry.set_posicao_e_rotacao(vec3(hx, 0.f, hz), theta_harry);
        jDraco.set_posicao_e_rotacao(vec3(dx, 0.f, dz), theta_draco);
        controladorJogo.resetarJogo();
        pulo_tecla_liberada_harry = true;
        pulo_tecla_liberada_draco = true;
        flag_perspectiva = 1;
        break;
    }
    case 'v':
    case 'V':
        flag_perspectiva = 1;
        break;
    case 'b':
    case 'B':
        flag_perspectiva = 0;
        break;
    case 27 :
         exit(0);
    }
    glutPostRedisplay();
}

void mouse(int button, int state, int x, int y){
    if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN){
        lastX = x;
        lastY = y;
        mouse_direita = 1;
    }
    if (button == GLUT_RIGHT_BUTTON && state == GLUT_UP) {
        mouse_direita = 0;
    }

    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) mouse_esquerda = 1;
    else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) mouse_esquerda = 0; 

    glutPostRedisplay();
}

void mouse_motion(int x, int y)
{
    mouse_x_diff = x - lastX; 
    mouse_y_diff = y - lastY; 

    

    if (mouse_direita)
    {
        camXZAngle -= x - lastX;
        if (camXZAngle < -180) camXZAngle = -180;
        else if (camXZAngle > 180) camXZAngle = 180;
        
        camXYAngle += y - lastY;
        if (camXYAngle < -89.9) camXYAngle = -89.9;
        else if (camXYAngle > 89.9) camXYAngle = 89.9;
    }    

    lastX = x;
    lastY = y;
    glutPostRedisplay();
}

void idle()
{
    GLdouble currentTimeElapsed = (GLdouble)glutGet(GLUT_ELAPSED_TIME);
    if (previousTime == 0)
        previousTime = currentTimeElapsed;
    GLdouble timeDiference = currentTimeElapsed - previousTime;
    previousTime = currentTimeElapsed;
    
    if (getControladorJogo().getEstado() != GAME_OVER)
        idle_jogo(timeDiference);

    currentTime = (int)currentTimeElapsed;
    int fatorTempo = 17;
    if (currentTime - lastTime > fatorTempo) {
        lastTime = currentTime;
        updateDrawing = 1;
        /* Zoom contínuo ao segurar + ou - (key repeat desligado para o pulo; aplicamos aqui) */
        if (teclas['+']){ zoom++; teclas['+'] = 0;}
        if (teclas['-']){ zoom--; teclas['-'] = 0;}
    }

    glutPostRedisplay();
}

void idle_jogo(int timeDiference)
{
    
    /* Movimento: estado a partir das teclas (minúscula e maiúscula) */
    if (teclas['w'] || teclas['W'])
        jHarry.move(1);
    else if (teclas['s'] || teclas['S'])
        jHarry.move(-1);
    else
        jHarry.para();

    if (teclas['o'] || teclas['O'])
        jDraco.move(1);
    else if (teclas['l'] || teclas['L'])
        jDraco.move(-1);
    else
        jDraco.para();

    jHarry.atualiza_movimento(timeDiference);
    jDraco.atualiza_movimento(timeDiference);
    
    // Gira arma mouse
    jHarry.gira_arma(-mouse_x_diff*timeDiference*JOGADOR_POT_VEL_ANGULAR_MOUSE, mouse_y_diff*timeDiference*JOGADOR_POT_VEL_ANGULAR_MOUSE);
    mouse_x_diff = 0;
    mouse_y_diff = 0;
    // Gira arma teclado
    jDraco.gira_arma((teclas['4']-teclas['6'])*timeDiference*JOGADOR_POT_VEL_ANGULAR_MOUSE, (teclas['2']-teclas['8'])*timeDiference*JOGADOR_POT_VEL_ANGULAR_MOUSE);
    
    
    // Atira
    static int gatilho_harry = 0;
    if (mouse_esquerda)
    {
        gatilho_harry = 1;
    }
    else if (!mouse_esquerda && gatilho_harry)
    {
        //cout << "POW!\n\n";
        jHarry.atira();
        gatilho_harry = 0;
    }

    static int gatilho_draco = 0;
    if (teclas['5'])
    {
        gatilho_draco = 1;
    }
    else if (!teclas['5'] && gatilho_draco)
    {
        //cout << "POW!\n\n";
        jDraco.atira();
        gatilho_draco = 0;
    }
    

    idle_tiros(jHarry.retorna_tiros(), jDraco, timeDiference);
    idle_tiros(jDraco.retorna_tiros(), jHarry, timeDiference);

    /* Resolução de colisões: arena (atrator), pilastras (repulsores), jogador vs jogador */
    if (dadosArenaSVG.ok) {
        vec3 centro_arena((GLfloat)dadosArenaSVG.arena_cx, 0.f, (GLfloat)dadosArenaSVG.arena_cy);
        GLfloat raio_arena = (GLfloat)dadosArenaSVG.arena_r;
        jHarry.aplica_colisao_arena(centro_arena, raio_arena);
        jDraco.aplica_colisao_arena(centro_arena, raio_arena);
    }
    jHarry.aplica_colisao_obstaculos(obstaculos);
    jDraco.aplica_colisao_obstaculos(obstaculos);
    jHarry.aplica_colisao_inimigo(jDraco);

    GLdouble segundos = timeDiference / 1000.0;
    jHarry.gravidade((GLfloat)segundos, obstaculos, &jDraco);
    jDraco.gravidade((GLfloat)segundos, obstaculos, &jHarry);

    /* Rotação apenas no chão (no ar só frente/trás); minúscula e maiúscula */
    if ((teclas['a'] || teclas['A']) && !jHarry.esta_no_ar())
        jHarry.gira_corpo((GLfloat)(JOGADOR_VEL_ANGULAR * segundos));
    if ((teclas['d'] || teclas['D']) && !jHarry.esta_no_ar())
        jHarry.gira_corpo((GLfloat)(-JOGADOR_VEL_ANGULAR * segundos));
    if ((teclas['k'] || teclas['K']) && !jDraco.esta_no_ar())
        jDraco.gira_corpo((GLfloat)(JOGADOR_VEL_ANGULAR * segundos));
    if ((teclas[0xE7] || teclas[0xC7]) && !jDraco.esta_no_ar())  /* Ç minúsculo (0xE7) e maiúsculo (0xC7) */
        jDraco.gira_corpo((GLfloat)(-JOGADOR_VEL_ANGULAR * segundos));
}
/*
int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize (janela_largura,janela_altura);
    glutInitWindowPosition (0, 0);
    glutCreateWindow ("Jogo 3D");
    init();
    glutIdleFunc(idle);
    glutDisplayFunc(display);
    glutMotionFunc(mouse_motion);
    glutMouseFunc(mouse);
    glutMainLoop();
    return 0;
}
*/


void camera_terceira_pessoa_draco()
{
     glMatrixMode (GL_PROJECTION);
        glLoadIdentity();
        gluPerspective (45, (GLfloat)VIEWPORT_LARGURA/(GLfloat)VIEWPORT_DOWN_ALTURA, 1, 1000);
    glMatrixMode(GL_MODELVIEW);
    /* Offset da câmera (zoom e ângulos); rotação da câmera é independente dos personagens (mouse). */
    GLfloat ex = (GLfloat)(zoom*sin(camXZAngle*M_PI/180)*cos(camXYAngle*M_PI/180));
    GLfloat ey = (GLfloat)(zoom*sin(camXYAngle*M_PI/180));
    GLfloat ez = (GLfloat)(zoom*cos(camXZAngle*M_PI/180)*cos(camXYAngle*M_PI/180));

    vec3 d = jDraco.pos();
    gluLookAt(d.x()-ex, d.y()+ey, d.z()-ez, d.x(), d.y(), d.z(), 0.f, 1.f, 0.f);
    
    glTranslatef(0.f, -jDraco.altura()/2, 0.f);
}

void camera_terceira_pessoa_harry()
{
     glMatrixMode (GL_PROJECTION);
        glLoadIdentity();
        gluPerspective (45, (GLfloat)VIEWPORT_LARGURA/(GLfloat)VIEWPORT_DOWN_ALTURA, 1, 1000);
    glMatrixMode(GL_MODELVIEW);
    /* Offset da câmera (zoom e ângulos); rotação da câmera é independente dos personagens (mouse). */
    GLfloat ex = -(GLfloat)(zoom*sin(camXZAngle*M_PI/180)*cos(camXYAngle*M_PI/180));
    GLfloat ey = (GLfloat)(zoom*sin(camXYAngle*M_PI/180));
    GLfloat ez = -(GLfloat)(zoom*cos(camXZAngle*M_PI/180)*cos(camXYAngle*M_PI/180));

    vec3 d = jHarry.pos();
    gluLookAt(d.x()-ex, d.y()+ey, d.z()-ez, d.x(), d.y(), d.z(), 0.f, 1.f, 0.f);
    
    glTranslatef(0.f, -jHarry.altura()/2, 0.f);
}


void idle_tiros(std::list<Tiro3d*>& tiros_aliado, Jogador3d& inimigo, GLdouble t_dif)
{
    for (auto tiro_aliado = tiros_aliado.begin(); tiro_aliado != tiros_aliado.end(); ++tiro_aliado) {
        //cout << "oi\n";
        if((*tiro_aliado)->valido(t_dif/1000))
        {
            //cout << "oi2\n";
            (*tiro_aliado)->move(TIRO_VELOCIDADE*(t_dif/1000));
            
            bool colidiu = false;
            bool colidiuDano = false;
            colidiu = colidiuDano = colidiu || (*(*tiro_aliado)).verifica_colisao_jogador(inimigo);
            colidiu = colidiu || (*(*tiro_aliado)).verifica_colisao_obstaculos(obstaculos);
            
           //cout << colidiu << "\n";

            if (colidiu)
            {
                if (colidiuDano)
                {
                    int vidasAntes = inimigo.vidas();

                    if (&inimigo == &jHarry)
                        placar.notificarDanoEsquerda(vidasAntes);
                    else
                        placar.notificarDanoDireita(vidasAntes);
            
                }
                
                auto aux = (*tiro_aliado);
                tiro_aliado = tiros_aliado.erase(tiro_aliado);  
                delete aux;
            }
                
        }
        else
        { 
            auto aux = (*tiro_aliado);
            tiro_aliado = tiros_aliado.erase(tiro_aliado);  
            delete aux;
        }
    }
}

void configura_lanterna(int luz, GLfloat luz_difusa[4], GLfloat luz_especular[4] ){
    glEnable(GL_LIGHTING);
    
    glEnable(luz);

    glLightfv(luz, GL_DIFFUSE, luz_difusa);
    glLightfv(luz, GL_SPECULAR, luz_especular);

    glLightf(luz, GL_SPOT_CUTOFF, 45.0f);

    glLightf(luz, GL_SPOT_EXPONENT, 50.0f); 
    
    glLightf(luz, GL_CONSTANT_ATTENUATION, 1.0f);
    glLightf(luz, GL_LINEAR_ATTENUATION, 0.05f);
    glLightf(luz, GL_QUADRATIC_ATTENUATION, 0.001f);

    glDisable(luz);
}

