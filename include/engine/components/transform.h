#ifndef GAME_TRANSFORM_H
#define GAME_TRANSFORM_H

#include <glm/vec2.hpp>

#include "../base.h"
#include "../parser.h"

namespace herb {

struct Transform : Component
{
    glm::vec2 position = {0, 0};
    glm::vec2 scale = {1, 1};

    static Component *
    deserialize(herb::Parser &parser)
    {
        auto t = new Transform;
        t->position = parser.parseVector2("position");
        t->scale = parser.parseVector2("scale");

        return t;
    }
};

} // namespace herb

#endif
