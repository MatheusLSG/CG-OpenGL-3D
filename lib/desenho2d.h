#ifndef DESENHO2D_H
#define DESENHO2D_H

#include <GL/gl.h>
#include <math.h>

/**
 * @brief Desenha um coração 2D em coordenadas de janela (projeção ortográfica).
 * @param cx Coordenada X do centro.
 * @param cy Coordenada Y do centro.
 * @param radius Tamanho (escala) do coração. A cor deve ser definida antes via glColor3f/glColor4f.
 */
inline void desenhaCoracao2D(GLfloat cx, GLfloat cy, GLfloat radius)
{
    const int segmentos = 40;

    glBegin(GL_TRIANGLE_FAN);
        glVertex2f(cx, cy);

        for (int i = 0; i <= segmentos; ++i) {
            float t = (float)i / (float)segmentos * 2.0f * 3.1415926f;
            /* Parametrização clássica de coração 2D */
            float x = 16.0f * powf(sinf(t), 3.0f);
            float y = 13.0f * cosf(t) - 5.0f * cosf(2.0f * t)
                      - 2.0f * cosf(3.0f * t) - cosf(4.0f * t);

            x *= radius / 18.0f;
            y *= radius / 18.0f;

            glVertex2f(cx + x, cy + y);
        }
    glEnd();
}

/**
 * @brief Desenha animação de coração explodindo em 4 frames (0 = normal, 1 = inflado, 2 = quebrando, 3 = estilhaços).
 * @param cx Coordenada X do centro.
 * @param cy Coordenada Y do centro.
 * @param radius Tamanho base.
 * @param frame Frame da animação (0 a 3).
 */
inline void desenhaCoracaoExplodindo2D(GLfloat cx, GLfloat cy, GLfloat radius, int frame)
{
    if (frame <= 0) {
        desenhaCoracao2D(cx, cy, radius);
        return;
    }

    if (frame > 3) {
        frame = 3;
    }

    const GLfloat escalaFrame[4] = { 1.0f, 1.2f, 0.9f, 0.6f };
    GLfloat escala = escalaFrame[frame];

    if (frame < 3) {
        desenhaCoracao2D(cx, cy, radius * escala);
    }

    const int numEstilhacos = 6;
    const GLfloat tamBase = radius * 0.25f;

    glBegin(GL_QUADS);
    for (int i = 0; i < numEstilhacos; ++i) {
        float ang = (2.0f * 3.1415926f / numEstilhacos) * i;
        GLfloat dist = radius * (0.3f * frame);
        GLfloat ex = cx + cosf(ang) * dist;
        GLfloat ey = cy + sinf(ang) * dist;
        GLfloat half = tamBase * (1.0f - 0.2f * frame);

        glVertex2f(ex - half, ey - half);
        glVertex2f(ex + half, ey - half);
        glVertex2f(ex + half, ey + half);
        glVertex2f(ex - half, ey + half);
    }
    glEnd();
}

#endif // DESENHO2D_H

