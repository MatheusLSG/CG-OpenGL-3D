#if !defined(LEITOR_DE_SVG_H)
#define LEITOR_DE_SVG_H

#include <vector>
#include <string>

/**
 * @brief Círculo 2D; coordenadas (cx, cy) do SVG são mapeadas para (x, z) no mundo 3D.
 */
struct Circulo2D {
    float cx;
    float cy;
    float r;
};

/**
 * @brief Dados da arena extraídos do SVG, escalados para o mundo do jogo. Altura (eixo Y) não vem do SVG.
 */
struct DadosArenaSVG {
    bool ok;
    float arena_cx;
    float arena_cy;
    float arena_r;
    float harry_x;
    float harry_z;
    float harry_r;
    float draco_x;
    float draco_z;
    float draco_r;
    std::vector<Circulo2D> pilastras;
};

/**
 * @brief Carrega o SVG e preenche os dados da arena; usa o círculo azul para escalar (raio no mundo = raioArenaMundo).
 * @param path Caminho do arquivo SVG.
 * @param raioArenaMundo Raio desejado da arena no mundo 3D.
 * @param out Estrutura preenchida em caso de sucesso.
 * @return true se o carregamento e parsing foram bem-sucedidos.
 */
bool carregaArenaSVG(const char* path, float raioArenaMundo, DadosArenaSVG* out);

#endif // LEITOR_DE_SVG_H
