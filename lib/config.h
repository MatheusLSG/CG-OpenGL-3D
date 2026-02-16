#if !defined(_CONFIG_H_)
#define _CONFIG_H_

#include "../lib/vec3.h"
#include <GL/gl.h>

/** Constantes de cores, dimensões da janela, parâmetros do jogo e do jogador. */

/* Cores (macros que expandem para cor(r,g,b)) */

#define VERMELHO            cor(0.9, 0, 0)
#define VERMELHO_CLARO      cor(1, 0, 0)
#define VERMELHO_ESCURO     cor(0.6, 0, 0)
#define VERDE               cor(0, 0.6, 0)
#define VERDE_CLARO         cor(0, 1, 0)
#define VERDE_ESCURO        cor(0, 0.4, 0)
#define AZUL                cor(0, 0, 0.8)
#define PRETO               cor(0, 0, 0)
#define BRANCO              cor(1, 1, 1)

/** Quando true, modelos não usam emissão (cena fica totalmente escura sem luz). Definido em main.cpp. */
extern bool forcar_zero_emissao;


/* Janela e viewport */
#define  JANELA_LARGURA    (GLint) 800
#define  JANELA_ALTURA     (GLint) 500

#define  VIEWPORT_LARGURA  (GLint) 500
#define  VIEWPORT_ALTURA   (GLint) 500

#define JOGADOR_VEL_DIRECIONAL (GLfloat) 125
#define JOGADOR_VEL_ANGULAR (GLfloat) 135
#define JOGADOR_VEL_ANIMACAO (GLfloat) 360
#define JOGADOR_PROP_PARADA 1.01

#define JOGADOR_BRACO_VEL_ANGULAR (GLfloat) 225
#define JOGADOR_POT_VEL_ANGULAR_MOUSE 5

#define JOGADOR_ALTURA (GLfloat) 80.0

#define OBSTACULO_ALTURA_MULTIPLICADOR 4

#define ARENA_RAIO_PADRAO (GLfloat) 250
#define ARENA_X_PADRAO (GLfloat) 0
#define ARENA_Y_PADRAO (GLfloat) 0

#define TIRO_VELOCIDADE (GLfloat) 2*JOGADOR_VEL_DIRECIONAL
#define TIRO_TEMPO_LIMITE 10

#define COLISAO_MARGEM (GLfloat) 0.01

#define TEXTO_FONTE GLUT_BITMAP_9_BY_15

#define TEXTO_FONTE_ALTURA      15
#define TEXTO_FONTE_LARGURA     9

#define VIDAS_INICIAL   3

#define PONTOS_POR_ELIPSE 100

/** Estados de animação do jogador (parado, andando, correndo, pulando, atacando, etc.). */
enum Estado{
    PARADO,
    ANDANDOFRENTE,
    ANDANDOTRAS,
    CORRENDOFRENTE,
    CORRENDOTRAS,
    PULANDO,
    CAINDO,
    ATACANDO,
    DANO,
    GANHAR,
    PERDER
};

#define PERMITE_COLISAO_ENTRE_TIROS false

#endif // _CONFIG_H_
