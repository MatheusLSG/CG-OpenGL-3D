#if !defined(_TRANSFORM_H_)
#define _TRANSFORM_H_

#include "../lib/vec3.h"
#include <GL/gl.h>

/**
 * @brief Translada o ponto por (dx, dy, dz).
 * @param dx, dy, dz Deslocamento.
 * @param ponto Ponto original.
 * @return Novo ponto transladado.
 */
vec3 translacao3D(GLfloat dx, GLfloat dy, GLfloat dz, vec3 ponto);

/**
 * @brief Rotação em torno do eixo X.
 * @param ang Ângulo em graus.
 * @param ponto Ponto a rotacionar.
 * @return Ponto rotacionado.
 */
vec3 rotacao3Dx(GLfloat ang, vec3 ponto);

/**
 * @brief Rotação em torno do eixo Y.
 * @param ang Ângulo em graus.
 * @param ponto Ponto a rotacionar.
 * @return Ponto rotacionado.
 */
vec3 rotacao3Dy(GLfloat ang, vec3 ponto);

/**
 * @brief Rotação em torno do eixo Z.
 * @param ang Ângulo em graus.
 * @param ponto Ponto a rotacionar.
 * @return Ponto rotacionado.
 */
vec3 rotacao3Dz(GLfloat ang, vec3 ponto);

#endif // _TRANSFORM_H_
