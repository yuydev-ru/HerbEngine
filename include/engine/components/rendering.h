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
    Image originalImage;
    Image image;
    Texture texture;

    static Component *
    deserialize(herb::Parser &parser)
    {
        auto spr = new Sprite;

        spr->assetPath = parser.parseElement<std::string>("assetPath");
        spr->originalImage = LoadImage(spr->assetPath.c_str());
        spr->image = ImageCopy(spr->originalImage);
        spr->texture = LoadTextureFromImage(spr->image);

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
