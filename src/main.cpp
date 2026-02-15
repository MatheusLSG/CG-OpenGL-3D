#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <math.h>
#include <cstring>
#include <stdlib.h>
#include "../lib/objloader.h"
#include "../lib/main.h"
using namespace std;
//Tela
int window_width = 800;
int window_height = 500;


//Malhas a serem desenhadas
meshes harry;
int harryIdle = -1;

meshes draco; 
int dracoActing = 0;
int dracoIdle = -1;
int dracoWalk = -1;
int dracoRun = -1;
int dracoJump = -1;

int dracoState = -1;

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

void init ()
{
    glShadeModel (GL_SMOOTH);
    glEnable(GL_LIGHTING);  
    glEnable(GL_LIGHT0);
    glEnable(GL_DEPTH_TEST);

    glMatrixMode (GL_PROJECTION);
        glLoadIdentity();
        gluPerspective (45, (GLfloat)1/(GLfloat)1, 1, 1000);
    glMatrixMode(GL_MODELVIEW);
    
    //Carrega as meshes dos arquivos
    dracoIdle = draco.loadMeshAnim("resources/draco/dracoIdle/dracoIdle####.obj", 383, 1);
    dracoWalk = draco.loadMeshAnim("resources/draco/dracoWalk/dracoWalk####.obj", 86, 1);
    
    draco.drawInit(dracoIdle);

    vector<string> dracoTexturesPaths;
    dracoTexturesPaths.push_back("resources/draco/texture/dracoTex0.bmp");
    dracoTexturesPaths.push_back("resources/draco/texture/dracoTex1.bmp");
    dracoTexturesPaths.push_back("resources/draco/texture/dracoTex2.bmp");

    if( !draco.loadTexture(dracoTexturesPaths) ) exit(printf("Lista de texturas invalidas!\n"));
    
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

        //Escolhe entre iniciar o desenho do chute ou soco
        if (lookatToggle && dracoState != dracoWalk){
            draco.drawInit(dracoWalk);
            dracoState = dracoWalk;
        } else if (armaToggle && dracoState != dracoRun){
            draco.drawInit(dracoRun);
            dracoState = dracoRun;
        } else if (transformacaoArmaToggle && dracoState != dracoJump){
            draco.drawInit(dracoJump);
            dracoState = dracoJump;
        }


        // ViewportHarry
        glViewport(0, 0, window_width/2, window_height);
        glPushMatrix();
        
        //DesenhoMundo

            //harry.draw(0, 0);
            
            glTranslatef(0, 0, 100);
            glRotatef(180, 0,1,0);

            //desenhar draco
            draco.drawCurrent();

            if (coordsysToggle == 1)  DrawAxes(83);
    
        glPopMatrix();
            
        // ViewportDraco
        glViewport(window_width/2, 0, window_width/2, window_height);
        
        glPushMatrix();

            //camera
            glTranslatef(0, 0, 100);
            glRotatef(180, 0,1,0);

            //DesenhoMundo
            //harry.draw(0,0);
            
            glTranslatef(0, 0, 100);
            glRotatef(180, 0,1,0);
            
                //Desanha draco
            int rtn = draco.drawNext();
            
            if (rtn){
                draco.drawInit(dracoIdle);
                dracoState = -1;
            }
            
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
        transformacaoArmaToggle = 1;
        break;
    case '2':
        transformacaoArmaToggle = 2;
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

    int fatorTempo = 10;
    if (currentTime - lastTime > fatorTempo){
        lastTime = currentTime;
        updateDrawing = 1;
    }
    

    glutPostRedisplay();
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize (window_width,window_height);
    glutInitWindowPosition (0, 0);
    glutCreateWindow ("Jogo 3D");
    init();
    glutIdleFunc(idle);
    glutDisplayFunc(display);
    glutKeyboardFunc(keyPress);
    glutMotionFunc(mouse_motion);
    glutMouseFunc(mouse);
    glutMainLoop();
    return 0;
}
