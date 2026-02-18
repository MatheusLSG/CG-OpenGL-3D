#include <GL/gl.h>
#include <GL/glut.h>
#include <string>
#include <vector>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include "../lib/objloader.h"
#include "../lib/config.h"
#include "../lib/imageloader.h"

using namespace std;

//Aponta para uma das meshes de movimento e coloca na primeira frame
void meshes::drawInit(int movID){
    currentMovID = movID;
    currentFrame = 0;
}

//Desenha a proxima frame do movimento atual e retorna se ja chegou no final
bool meshes::next(){
    this->currentFrame++;
    
    return (this->currentFrame >= this->vecMeshes[this->currentMovID].size()-1);
}

//Desenha a frame atual
void meshes::drawCurrent(){
    this->draw(this->currentMovID, this->currentFrame);
}

//Carrega as meshes de um movimento do caminho path e assumindo qtd arquivos
int meshes::loadMeshAnim(string path, int qtd, int none){
    
    vector<mesh> vec;
    mesh m;
    this->vecMeshes.push_back(vec);
    int movID = this->vecMeshes.size()-1;
    char str[5];
    size_t index = path.find("#");
    for(int i = 0; i<qtd; i++){
        snprintf (str, 11, "%04d", i);
        path.replace(index, 4, str);
        // std::cout << path << std::endl;  // debug: prints de objetos comentados
        this->vecMeshes[movID].push_back(m);
        this->vecMeshes[movID][this->vecMeshes[movID].size()-1].loadMesh(path, none);
    }
    return movID;
    
}

//Desenha uma mesh com a respectiva textura
void meshes::draw(int movID, int frameId){
    this->vecMeshes[movID][frameId].draw(this->texIDs);
}

//Le a textura
bool meshes::loadTexture(vector<string> paths, vector<int> transparente){
    int n = paths.size();
    
    if (!n) return 0;

    this->texIDs =  (GLuint*) calloc( n, sizeof(GLuint));
    
    glGenTextures(n, this->texIDs );

    
    

    for (int i = 0; i < n; i++)
    {
        FILE* file= fopen(paths[i].data(), "r");
        
        Image* image = loadBMP(paths[i].c_str());
        

        this->texWidth = image->width;
        this->texHeight = image->height;
        
        int formato; 
        if (transparente[i]) formato = GL_RGBA;
        else  formato = GL_RGB;

        glBindTexture( GL_TEXTURE_2D, this->texIDs[i] );
        //glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

        glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE,GL_REPLACE );
        glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_LINEAR );
        glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_LINEAR );
        glTexImage2D(GL_TEXTURE_2D,                //Always GL_TEXTURE_2D
                                0,                            //0 for now
                                GL_RGBA,                       //Format OpenGL uses for image
                                this->texWidth, this->texHeight,  //Width and height
                                0,                            //The border of the image
                                formato,                         //Src format
                                GL_UNSIGNED_BYTE, //GL_UNSIGNED_BYTE, because pixels are stored
                                                //as unsigned numbers
                                image->pixels);               //The actual pixel data
                    
        delete image;
    }
    
    return 1;
}

//função para carregar o OBJ
bool mesh::loadMesh(string path, int none){
    vector<int> vertIndex, uvIndex, normIndex;//indexadores para vertices, mapeamento de textura e normais
    vertsPos.clear();
    vertsNorm.clear();
    vertsTex.clear();
    vertsS.clear();
    int i = -1;
    int tIdx = -1;
    int v_util_now = 0;
    FILE* file= fopen(path.data(), "r");
    if(file== NULL){
        // cout<< "falha ao carregar o arquivo"<< endl;  // debug: comentado
        return false;
    }
    else{ 
        while(1){
            char lineHeader[128];
            int res= fscanf(file, "%s", lineHeader);
            if(res==EOF){
                    break;
            }
            if(strcmp(lineHeader, "v") == 0){
                pos vert;
                fscanf(file, "%f %f %f\n", &vert.x, &vert.y, &vert.z);
                vertsPos.push_back(vert);
            }
            else if(strcmp(lineHeader,"vt")==0){
                tex vert;
                fscanf(file, "%f %f\n", &vert.u, &vert.v);
                vertsTex.push_back(vert);
            }
            else if(strcmp(lineHeader, "vn")==0){
                norm vert;
                fscanf(file, "%f %f %f\n", &vert.x, &vert.y, &vert.z);
                vertsNorm.push_back(vert);
            }
            else if(strcmp(lineHeader, "f")==0){
                string v1, v2, v3;
                unsigned int vertInd[3], uvInd[3], normInd[3];
                int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertInd[0], &uvInd[0], &normInd[0], &vertInd[1], &uvInd[1], &normInd[1], &vertInd[2], &uvInd[2], &normInd[2]);
                if(matches !=9){
                        // printf("QUantidade de valores lidos diferente do esperado. Falha ao ler uma linha de face");  // debug: comentado
                        return false;			
                }
                //Cria uma lista com os índices na ordem apropriada para o desenho das faces
                //Esta é a lista de índices de vértices
                vertIndex.push_back(vertInd[0]);
                vertIndex.push_back(vertInd[1]);
                vertIndex.push_back(vertInd[2]);
                //Esta é a lista de índices de mapeamento de textura
                uvIndex.push_back(uvInd[0]);
                uvIndex.push_back(uvInd[1]);
                uvIndex.push_back(uvInd[2]);
                // Esta é a lista de índices de normais
                normIndex.push_back(normInd[0]);
                normIndex.push_back(normInd[1]);
                normIndex.push_back(normInd[2]);
            }
            else if(strcmp(lineHeader, "o")==0){
                if(tIdx != -1)
                {
                    for(unsigned int i=v_util_now; i<vertIndex.size(); i++){
                        verticeStrip vert;
                        vert.vPos.x = vertsPos[vertIndex[i]-1].x;
                        vert.vPos.y = vertsPos[vertIndex[i]-1].y;
                        vert.vPos.z = vertsPos[vertIndex[i]-1].z;
                        vert.vTex.u = vertsTex[uvIndex[i]-1].u;
                        vert.vTex.v = vertsTex[uvIndex[i]-1].v;
                        vert.vNorm.x = vertsNorm[normIndex[i]-1].x;
                        vert.vNorm.y = vertsNorm[normIndex[i]-1].y;
                        vert.vNorm.z = vertsNorm[normIndex[i]-1].z;
                        vert.idx = tIdx;
                        vertsS.push_back(vert);
                    }
                    v_util_now = vertIndex.size();
                }

                fscanf(file, " %s", lineHeader);
                if (strcmp(lineHeader, "None") == 0)
                {
                    tIdx = none;
                } 
                else 
                {
                    tIdx = string(lineHeader).back() - '0';
                }
            }
        }
        for(unsigned int i=v_util_now; i<vertIndex.size(); i++){
            verticeStrip vert;
            vert.vPos.x = vertsPos[vertIndex[i]-1].x;
            vert.vPos.y = vertsPos[vertIndex[i]-1].y;
            vert.vPos.z = vertsPos[vertIndex[i]-1].z;
            vert.vTex.u = vertsTex[uvIndex[i]-1].u;
            vert.vTex.v = vertsTex[uvIndex[i]-1].v;
            vert.vNorm.x = vertsNorm[normIndex[i]-1].x;
            vert.vNorm.y = vertsNorm[normIndex[i]-1].y;
            vert.vNorm.z = vertsNorm[normIndex[i]-1].z;
            vert.idx = tIdx;
            vertsS.push_back(vert);
        }
        
    }
    return true;
}

//desenha a malha
void mesh::draw(GLuint *textIDs){

    int cont=0;
    GLfloat materialEmission[] = { 0.10, 0.10, 0.10, 1};
    GLfloat emissionZero[] = { 0, 0, 0, 1 };
    GLfloat materialColorA[] = { 0.1, 0.1, 0.1, 0.1};
    GLfloat materialColorD[] = { .90, .90, .90, 1};
    glColor3f(1,1,1);

    glMaterialfv(GL_FRONT, GL_EMISSION, forcar_zero_emissao ? emissionZero : materialEmission);
    glMaterialfv(GL_FRONT, GL_AMBIENT, materialColorA);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, materialColorD);

    for(unsigned int i=0; i<vertsS.size(); i=i+3){
        if (textIDs != NULL){
            glEnable(GL_TEXTURE_2D);
            glBindTexture (GL_TEXTURE_2D, textIDs[vertsS[i].idx]);
        }

        glBegin(GL_TRIANGLE_STRIP);
            for(int j=i; j<i+3; j++){
                

                glTexCoord2f (vertsS[j].vTex.u, vertsS[j].vTex.v);
                glNormal3f(vertsS[j].vNorm.x,vertsS[j].vNorm.y,vertsS[j].vNorm.z);
                glVertex3f(vertsS[j].vPos.x,vertsS[j].vPos.y,vertsS[j].vPos.z);
                
            }
        glEnd();

        
        if (textIDs != NULL){
            glDisable(GL_TEXTURE_2D);
        }
    }
}
