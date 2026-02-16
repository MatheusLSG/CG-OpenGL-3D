#include "../lib/leitorDeSVG.h"
#include "../lib/tinyxml2.h"
#include <cctype>
#include <cmath>
#include <cstring>

using namespace tinyxml2;

static bool string_equals_ignore_case(const char* a, const char* b)
{
    if (!a || !b) return (a == b);
    while (*a && *b) {
        if (tolower((unsigned char)*a) != tolower((unsigned char)*b))
            return false;
        ++a; ++b;
    }
    return (*a == *b);
}

bool carregaArenaSVG(const char* path, float raioArenaMundo, DadosArenaSVG* out)
{
    if (!path || !out)
        return false;

    out->ok = false;
    out->arena_cx = out->arena_cy = out->arena_r = 0.f;
    out->harry_x = out->harry_z = out->harry_r = 0.f;
    out->draco_x = out->draco_z = out->draco_r = 0.f;
    out->pilastras.clear();

    XMLDocument doc;
    if (doc.LoadFile(path) != XML_SUCCESS)
        return false;

    const XMLElement* root = doc.RootElement();
    if (!root)
        return false;

    float arena_cx_svg = 0.f, arena_cy_svg = 0.f, arena_r_svg = 0.f;
    bool arena_encontrada = false;

    for (const XMLElement* circle = root->FirstChildElement("circle"); circle;
         circle = circle->NextSiblingElement("circle")) {
        const char* fill = circle->Attribute("fill");
        if (!fill) continue;
        if (!string_equals_ignore_case(fill, "blue")) continue;

        arena_cx_svg = circle->FloatAttribute("cx", 0.f);
        arena_cy_svg = circle->FloatAttribute("cy", 0.f);
        arena_r_svg  = circle->FloatAttribute("r",  0.f);
        if (arena_r_svg <= 0.f)
            return false;
        arena_encontrada = true;
        break;
    }

    if (!arena_encontrada)
        return false;

    float scale = raioArenaMundo / arena_r_svg;
    out->arena_cx = 0.f;
    out->arena_cy = 0.f;
    out->arena_r  = raioArenaMundo;

    for (const XMLElement* circle = root->FirstChildElement("circle"); circle;
         circle = circle->NextSiblingElement("circle")) {
        const char* fill = circle->Attribute("fill");
        if (!fill) continue;

        float cx_svg = circle->FloatAttribute("cx", 0.f);
        float cy_svg = circle->FloatAttribute("cy", 0.f);
        float r_svg  = circle->FloatAttribute("r",  0.f);

        float wx = (cx_svg - arena_cx_svg) * scale;
        float wz = (cy_svg - arena_cy_svg) * scale;
        float wr = r_svg * scale;

        if (string_equals_ignore_case(fill, "red")) {
            out->harry_x = wx;
            out->harry_z = wz;
            out->harry_r = wr;
        } else if (string_equals_ignore_case(fill, "green")) {
            out->draco_x = wx;
            out->draco_z = wz;
            out->draco_r = wr;
        } else if (string_equals_ignore_case(fill, "black")) {
            Circulo2D p;
            p.cx = wx;
            p.cy = wz;
            p.r  = wr;
            out->pilastras.push_back(p);
        }
    }

    out->ok = true;
    return true;
}
