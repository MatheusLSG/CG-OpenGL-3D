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

/** Velocidade do jogador ao andar (unidades por segundo). Multiplicada pelo delta de tempo para ser independente da taxa de quadros. */
#define JOGADOR_VELOCIDADE (GLfloat) 125.0
#define JOGADOR_VEL_DIRECIONAL (GLfloat) 125
/** Velocidade angular de rotação do jogador (graus por segundo). */
#define JOGADOR_VEL_ANGULAR (GLfloat) 135
#define JOGADOR_VEL_ANIMACAO (GLfloat) 360
#define JOGADOR_PROP_PARADA 1.01

#define JOGADOR_BRACO_VEL_ANGULAR (GLfloat) 225
#define JOGADOR_POT_VEL_ANGULAR_MOUSE 5

#define JOGADOR_ALTURA (GLfloat) 80.0

/** Gravidade (unidades/s²). */
#define GRAVIDADE (GLfloat) JOGADOR_ALTURA
/** Altura máxima do pulo (pés do jogador) = 2x a altura do personagem; limitado na gravidade(). */
#define PULO_ALTURA_MAXIMA (GLfloat) (2.0 * JOGADOR_ALTURA)
/** Velocidade vertical inicial (atinge 2*H); limite rígido em PULO_ALTURA_MAXIMA garante que não passe. */
#define VELOCIDADE_PULO_INICIAL (GLfloat) (2.0 * GRAVIDADE)

#define OBSTACULO_ALTURA_MULTIPLICADOR 4
/** Altura do teto da arena (pés do jogador não podem fazer a cabeça passar desse valor). */
#define TETO_ARENA_ALTURA (GLfloat) (OBSTACULO_ALTURA_MULTIPLICADOR * JOGADOR_ALTURA)

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
