#ifndef GAME_TRANSFORM_H
#define GAME_TRANSFORM_H

#include "../base.h"
#include "../parsing.h"

#include <SFML/Graphics.hpp>


struct Transform : Component
{
    sf::Vector2f position = {0, 0};
    sf::Vector2f scale = {1, 1};

    static Component *
    deserialize(Parsing::configFile &dict)
    {
        auto t = new Transform;

        t->position = Parsing::parseVector2<float>(dict, "position");
        t->scale = Parsing::parseVector2<float>(dict, "scale");

        return t;
    }
};

#endif