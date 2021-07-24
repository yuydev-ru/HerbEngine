#ifndef ENGINE_RENDERING_H
#define ENGINE_RENDERING_H

#include "../base.h"
#include "../parsing.h"

#include <SFML/Graphics.hpp>

struct Sprite : Component
{
    std::string assetPath;
    sf::Image image;
    sf::Texture texture;
    sf::Sprite sprite;

    static Component *
    deserialize(Parsing::configFile &dict)
    {
        auto spr = new Sprite;

        spr->assetPath = Parsing::parseElement<std::string>(dict, "assetPath");

        spr->image.loadFromFile(spr->assetPath);
        spr->texture.loadFromImage(spr->image);
        spr->sprite.setTexture(spr->texture);
        sf::IntRect rect = { 0, 0
                , (int) spr->image.getSize().x
                , (int) spr->image.getSize().y };
        spr->sprite.setTextureRect(rect);

        return spr;
    }
};

struct Camera : Component
{
    sf::Vector2f scale = {1, 1};

    static Component *
    deserialize(Parsing::configFile &dict)
    {
        auto c = new Camera;

        c->scale = Parsing::parseVector2<float>(dict, "scale");

        return c;
    }
};

void
render(GameState *, Storage *, const Entity);

#endif
