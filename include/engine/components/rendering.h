#ifndef ENGINE_RENDERING_H
#define ENGINE_RENDERING_H

#include <glm/vec2.hpp>

#include <raylib.h>

#include "../base.h"
#include "../parser.h"

namespace herb {

struct Sprite : Component
{
    std::string assetPath;
    Texture texture;

    static Component *
    deserialize(herb::Parser &parser)
    {
        auto spr = new Sprite;

        spr->assetPath = parser.parseElement<std::string>("assetPath");
        // TODO(andrew): Убрать c_str
        spr->texture = LoadTexture(spr->assetPath.c_str());

        return spr;
    }
};

struct Camera : Component
{
    glm::vec2 scale = {1, 1};

    static Component *
    deserialize(herb::Parser &parser)
    {
        auto c = new Camera;
        c->scale = parser.parseVector2("scale");

        return c;
    }
};

void
render(herb::GameState *, herb::Storage *, const herb::Entity);

} // namespace herb

#endif
