#ifndef GAME_TRANSFORM_H
#define GAME_TRANSFORM_H

#include "../base.h"
#include "../parser.h"
#include <SFML/Graphics.hpp>


struct Transform : Component
{
    sf::Vector2f position = {0, 0};
    sf::Vector2f scale = {1, 1};

    static Component *
    deserialize(Parser &parser)
    {
        auto t = new Transform;
        t->position = parser.parseVector2<float>("position");
        t->scale = parser.parseVector2<float>("scale");

        return t;
    }
};

#endif