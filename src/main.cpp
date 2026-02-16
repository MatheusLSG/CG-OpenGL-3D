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

// arena

char *arquivo_arena;

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


// DEFINICAO DE FUNCOES ----------------------------------------------------------------------

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
    if (argc < 2 && 0)
    {
        std::cerr << "erro: numero de argumento invalido!\n";
        return 1;
    }

    //arquivo_arena = argv[1];
    
    // glut
    glutInit(&argc, argv);
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    
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
    glEnable(GL_LIGHT0);
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

    if( !draco.loadTexture(dracoTexturesPaths) ) exit(printf("Lista de texturas invalidas!\n"));
    
    jDraco = Jogador3d(
        10,
        180,
        vec3(0,0,0),
        VERDE,
        draco
    );


    //harry.loadMeshAnim("resources/harry/Default/harry.obj", 1);
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

    //x axis
    glPushMatrix();
        glMaterialfv(GL_FRONT, GL_EMISSION, mat_ambient_r);
        glColor3fv(mat_ambient_r);
        glScalef(size, size*0.1, size*0.1);
        glTranslatef(0.5, 0, 0); // put in one end
        glutSolidCube(1.0);
    glPopMatrix();

    //y axis
    glPushMatrix();
        glMaterialfv(GL_FRONT, GL_EMISSION, mat_ambient_g);
        glColor3fv(mat_ambient_g);
        glRotatef(90,0,0,1);
        glScalef(size, size*0.1, size*0.1);
        glTranslatef(0.5, 0, 0); // put in one end
        glutSolidCube(1.0);
    glPopMatrix();

    //z axis
    glPushMatrix();
        glMaterialfv(GL_FRONT, GL_EMISSION, mat_ambient_b);
        glColor3fv(mat_ambient_b);
        glRotatef(-90,0,1,0);
        glScalef(size, size*0.1, size*0.1);
        glTranslatef(0.5, 0, 0); // put in one end
        glutSolidCube(1.0);
    glPopMatrix();
    
}

void desenhaJogador(){
    

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    //INICIO
    //Translada para o centro do personagem para facilitar a rotacao da camera
        glTranslatef(0,-40,0);


        // ViewportHarry
        glViewport(0, 0, janela_largura/2, janela_altura);
        glPushMatrix();
                
            //camera
            glTranslatef(0, 0, -25);
            
            //Desenho Mundo

            //harry.draw(0, 0);
            
          
            //desenhar draco

            jDraco.desenha_jogador();

            if (coordsysToggle == 1)  DrawAxes(83);
    
        glPopMatrix();
            
        // ViewportDraco
        glViewport(janela_largura/2, 0, janela_largura/2, janela_altura);
        
        glPushMatrix();

            //camera
            glTranslatef(0, 0, 25);
            glRotatef(180, 0,1,0);

            //DesenhoMundo
            //harry.draw(0,0);
            

            jDraco.desenha_jogador();
            
            
            if (coordsysToggle == 1) DrawAxes(85);

        glPopMatrix();
        
    //FIM

    glPopMatrix();
}

void display(void)
{
    if (updateDrawing){
        updateDrawing = 0;
    } else{
        return;
    }
    
    jDraco.atualiza_animacao();

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

    //Define e desenha a fonte de luz

    GLfloat light_position[] = {(GLfloat) (zoom*sin(camXZAngle*M_PI/180)*cos((camXYAngle*M_PI/180))),
                                (GLfloat) (zoom*                         sin((camXYAngle*M_PI/180))),
                                (GLfloat) (zoom*cos(camXZAngle*M_PI/180)*cos((camXYAngle*M_PI/180))),
                                1};

    glLightfv(GL_LIGHT0,GL_POSITION,light_position);
    glDisable(GL_LIGHTING);
        glPointSize(15);
        glColor3f(1.0,1.0,0.0);
        glBegin(GL_POINTS);
            glVertex3f(light_position[0],light_position[1],light_position[2]);
        glEnd();    
    glEnable(GL_LIGHTING);
    
    glutSwapBuffers ();
}

void keyPress(unsigned char key, int x, int y)
{
    switch(key){
    case '1':
        jDraco.para();
        break;
    case '2':
        jDraco.move(1);
        break;
    case '3':
        jDraco.move(2);
        break;
    case '4':
        jDraco.move(-1);
        break;
    case '5':
        jDraco.move(-2);
        break;
    case '6':
        jDraco.pula();
        break;
    case '7':
        jDraco.atira();
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
