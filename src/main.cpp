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


// VARIAVEIS GLOBAIS

// dados iniciais

GLfloat j1_r = 0;
GLfloat j1_x = 0;
GLfloat j1_y = 0;

GLfloat j2_r = 0;
GLfloat j2_x = 0;
GLfloat j2_y = 0;

GLfloat arena_r = 0;
GLfloat arena_x = 0;
GLfloat arena_y = 0;

std::list<vec3> dados_obstaculos;

// placar

Placar placar;

// arena (path do SVG passado por linha de comando)
char* arquivo_arena = nullptr;

// Dados carregados do SVG (se arquivo foi passado). Mantém proporções; altura vem de jogador_altura_global.
DadosArenaSVG dadosArenaSVG;

// Altura do jogador no mundo (ditada por essa variável; usada para obstáculos 4x essa altura).
GLfloat jogador_altura_global = JOGADOR_ALTURA;

// teclas
int teclas[512];

// mouse
GLfloat mouse_pass = 0;
GLfloat mouse_mov = 0;

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


// OLD:

//Malhas a serem desenhadas
meshes harry;
int harryIdle = -1;




//Controle do tempo
//Soh trata maquinas mais rapidas, nao faz animacao relativa ao tempo
int currentTime = 0;
int lastTime = 0;
int updateDrawing = 0;

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
int buttonDown=0;
int flag_swap = 1;

GLfloat light_pos[3] = {0, 0 ,0};

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

void mouse_press(int botao, int estado, int x, int y);
void mouse_move(int x, int y);

void coleta_dados_arena();
void desenhaChaoAzul();
void desenhaCilindroJogadorDebug(vec3 pos, GLfloat raio, GLfloat altura, float r, float g, float b, float alpha);

GLuint geraTexturaDifusaParedes();

void idle_jogador_1(GLdouble t_dif);
void idle_jogador_2(GLdouble t_dif);
void idle_tiros(std::list<Tiro3d*>& tiros_aliado, std::list<Tiro3d*>& tiros_inimigo, Jogador3d& inimigo, GLdouble t_dif);

void renderiza_texto_direita(vec3 pos, cor cor_texto, char *texto);
void renderiza_texto_esquerda(vec3 pos, cor cor_texto, char *texto);
void renderiza_texto_centro(vec3 pos, cor cor_texto, char *texto);

//OLD
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
    glutMouseFunc(mouse);
    glutKeyboardFunc(keyPress);

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
    draco.drawInit(PARADO);
     
    vector<string> dracoTexturesPaths;
    dracoTexturesPaths.push_back("resources/draco/texture/dracoTex0.bmp");
    dracoTexturesPaths.push_back("resources/draco/texture/dracoTex1.bmp");
    dracoTexturesPaths.push_back("resources/draco/texture/dracoTex2.bmp");
    
    vector<int> dracoTransparente = {0,0,0};

    // if( !draco.loadTexture(dracoTexturesPaths, dracoTransparente) ) exit(printf("Lista de texturas invalidas!\n"));
    if( !draco.loadTexture(dracoTexturesPaths, dracoTransparente) ) { std::cerr << "Lista de texturas draco invalidas\n"; exit(1); }

    vec3 posDraco(0.f, 0.f, 0.f);
    if (dadosArenaSVG.ok) {
        posDraco = vec3(dadosArenaSVG.draco_x, 0.f, dadosArenaSVG.draco_z);
    }
    jDraco = Jogador3d(
        10,
        0,
        posDraco,
        VERDE,
        draco
    );

    meshes harry;
    
    harry.loadMeshAnim("resources/harry/idle/idle####.obj", 192, 1);
    harry.loadMeshAnim("resources/harry/walkforw/walkforw####.obj", 34, 1);
    harry.loadMeshAnim("resources/harry/walkback/walkback####.obj", 36, 1);
    harry.loadMeshAnim("resources/harry/runforw/runforw####.obj", 21, 1);
    harry.loadMeshAnim("resources/harry/runback/runback####.obj", 21, 1);
    harry.loadMeshAnim("resources/harry/jump/jump####.obj", 26, 1);
    harry.loadMeshAnim("resources/harry/fall/fall####.obj", 33, 1);
    harry.loadMeshAnim("resources/harry/atack/atack####.obj", 74, 1);
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
    if (dadosArenaSVG.ok) {
        posHarry = vec3(dadosArenaSVG.harry_x, 0.f, dadosArenaSVG.harry_z);
    }
    jHarry = Jogador3d(
        10,
        180,
        posHarry,
        VERMELHO,
        harry
    );

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
                alt,
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

// Chão azul só com specular (tipo vidro), sem textura difusa.
void desenhaChaoAzul()
{
    if (!dadosArenaSVG.ok) return;

    glPushMatrix();
    glTranslatef(dadosArenaSVG.arena_cx, 0.f, dadosArenaSVG.arena_cy);
    glRotatef(-90.f, 1.f, 0.f, 0.f);

    GLUquadric* quad = gluNewQuadric();
    gluQuadricNormals(quad, GLU_SMOOTH);

    GLfloat mat_amb_diff[4] = { AZUL.x(), AZUL.y(), AZUL.z(), 1.0f };
    GLfloat mat_spec[4]    = { 0.5f, 0.5f, 0.65f, 1.0f };
    GLfloat shininess[]    = { 90.0f };
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_amb_diff);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_spec);
    glMaterialfv(GL_FRONT, GL_SHININESS, shininess);
    glColor3f(AZUL.x(), AZUL.y(), AZUL.z());

    gluDisk(quad, 0.0, dadosArenaSVG.arena_r, PONTOS_POR_ELIPSE, 1);
    gluDeleteQuadric(quad);

    glPopMatrix();
}

void desenhaCilindroJogadorDebug(vec3 pos, GLfloat raio, GLfloat altura, float r, float g, float b, float alpha)
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_ALPHA_TEST);

    glPushMatrix();
    glTranslatef(pos.x(), pos.y(), pos.z());
    glRotatef(-90.f, 1.f, 0.f, 0.f);

    GLUquadric* quad = gluNewQuadric();
    gluQuadricNormals(quad, GLU_SMOOTH);
    glColor4f(r, g, b, alpha);
    GLfloat mat[4] = { r, g, b, alpha };
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat);
    gluCylinder(quad, raio, raio, altura, PONTOS_POR_ELIPSE, 4);
    gluDisk(quad, 0.0, raio, PONTOS_POR_ELIPSE, 1);
    glTranslatef(0.f, 0.f, altura);
    glRotatef(180.f, 1.f, 0.f, 0.f);
    gluDisk(quad, 0.0, raio, PONTOS_POR_ELIPSE, 1);
    gluDeleteQuadric(quad);

    glPopMatrix();

    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER, 0.5f);
    glDisable(GL_BLEND);
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

void desenhaJogador(){
    

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    // Translada para o centro do personagem para facilitar a rotacao da camera
    glTranslatef(0,-40,0);
    pomoDeOuro.atualizar(&dadosArenaSVG, OBSTACULO_ALTURA_MULTIPLICADOR * jogador_altura_global);
    forcar_zero_emissao = !pomoDeOuro.estaAtivo();
    if (!pomoDeOuro.estaAtivo()) {
        GLfloat zero[4] = { 0.f, 0.f, 0.f, 1.f };
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, zero);
        for (int i = 0; i < 8; i++) glDisable((GLenum)(GL_LIGHT0 + i));
    }

    // Viewport Harry
    glViewport(0, 0, janela_largura/2, janela_altura);
    glPushMatrix();
        glTranslatef(0, 0, -200);
        pomoDeOuro.aplicarLuz();

        desenhaChaoAzul();
        for (Obstaculo3d& o : obstaculos)
            o.desenha();
        if (pomoDeOuro.estaAtivo())
            pomoDeOuro.desenhar();

        glEnable(GL_ALPHA_TEST);
            glAlphaFunc(GL_GREATER, 0.5f);
            jHarry.desenha_jogador();
            jDraco.desenha_jogador();

            if (coordsysToggle == 1)  DrawAxes(83);
    
        glPopMatrix();
            
    // Viewport Draco: mesma câmera, centrada no Draco
    glViewport(janela_largura/2, 0, janela_largura/2, janela_altura);
    glPushMatrix();
        glLoadIdentity();
        {
            GLfloat ex = (GLfloat)(zoom*sin(camXZAngle*M_PI/180)*cos(camXYAngle*M_PI/180));
            GLfloat ey = (GLfloat)(zoom*sin(camXYAngle*M_PI/180));
            GLfloat ez = (GLfloat)(zoom*cos(camXZAngle*M_PI/180)*cos(camXYAngle*M_PI/180));
            gluLookAt(ex, ey, ez, 0.f, 0.f, 0.f, 0.f, 1.f, 0.f);
        }
        glTranslatef(-jDraco.pos().x(), -jDraco.pos().y(), -jDraco.pos().z());
        glTranslatef(0.f, -40.f, 0.f);
        pomoDeOuro.aplicarLuz();

        desenhaChaoAzul();
        for (Obstaculo3d& o : obstaculos)
            o.desenha();
        if (pomoDeOuro.estaAtivo())
            pomoDeOuro.desenhar();

        glEnable(GL_ALPHA_TEST);
            glAlphaFunc(GL_GREATER, 0.5f);
            jHarry.desenha_jogador();
            jDraco.desenha_jogador();

            if (coordsysToggle == 1) DrawAxes(85);

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
    if (updateDrawing) {
        updateDrawing = 0;
        jHarry.atualiza_animacao();
        jDraco.atualiza_animacao();
        ControladorJogo& cj = getControladorJogo();
        cj.atualizar(0.0);
    } else {
        return;
    }

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    //Controla camera
        //Limpa a cor com azulado
    glClearColor (0.30, 0.30, 1.0, 0.0);
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        //Utiliza uma esfera de raio zoom para guiar a posicao da camera
        //baseada em dois angulos (do plano XZ e do plano XY)
    gluLookAt(  zoom*sin(camXZAngle*M_PI/180)*cos((camXYAngle*M_PI/180)),
                zoom*                         sin((camXYAngle*M_PI/180)),
                zoom*cos(camXZAngle*M_PI/180)*cos((camXYAngle*M_PI/180)),
                0, 0, 0,
                0, 1, 0);

    
    desenhaJogador();

    getControladorJogo().desenharHUD();

    glutSwapBuffers ();
}

void keyPress(unsigned char key, int x, int y)
{
    ControladorJogo& controladorJogo = getControladorJogo();

    switch(key){
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
    case 'n':
    case 'N':
        pomoDeOuro.alternarAtivo();
        break;
    case 'd':
        if (jDraco.vidas() > 0 && controladorJogo.getEstado() == EM_PARTIDA) {
            int vidasAntes = jDraco.vidas();
            jDraco.dano();
            placar.notificarDanoDireita(vidasAntes);
        }
        break;
    case 'h':
        if (jHarry.vidas() > 0 && controladorJogo.getEstado() == EM_PARTIDA) {
            int vidasAntes = jHarry.vidas();
            jHarry.dano();
            placar.notificarDanoEsquerda(vidasAntes);
        }
        break;
    case 'a':
        armaToggle = !armaToggle;
        break;
    case 'c':
        coordsysToggle = !coordsysToggle;
        break;
    case 'm':
        lookatToggle = !lookatToggle;
        break;
    case 's':
        flag_swap = !flag_swap;
        break;
    case '+':
        zoom++;
        break;
    case '-':
        zoom--;
         break;
    case 'r':
    case 'R':
        controladorJogo.resetarJogo();
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
        buttonDown = 1;
    }
    if (button == GLUT_RIGHT_BUTTON && state == GLUT_UP) {
        buttonDown = 0;
    }
    glutPostRedisplay();
}

void mouse_motion(int x, int y)
{
    if (!buttonDown)
        return;
    
    camXZAngle -= x - lastX;
    camXYAngle += y - lastY;

    lastX = x;
    lastY = y;
    glutPostRedisplay();
}

void idle()
{
    // Elapsed time from the initiation of the game.
    currentTime = glutGet(GLUT_ELAPSED_TIME);

    int fatorTempo = 20;
    if (currentTime - lastTime > fatorTempo){
        lastTime = currentTime;
        updateDrawing = 1;
    }
    
    glutPostRedisplay();
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
