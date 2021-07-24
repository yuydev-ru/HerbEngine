#ifndef ENGINE_RENDERING_H
#define ENGINE_RENDERING_H

#include "../base.h"
#include "../parser.h"
#include <SFML/Graphics.hpp>

struct Sprite : Component
{
    std::string assetPath;
    sf::Image image;
    sf::Texture texture;
    sf::Sprite sprite;

    static Component *
    deserialize(Parser &parser)
    {
        auto spr = new Sprite;

        spr->assetPath = parser.parseElement<std::string>("assetPath");

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
    deserialize(Parser &parser)
    {
        auto c = new Camera;
        c->scale = parser.parseVector2<float>("scale");

        return c;
    }
};

void
render(GameState *, Storage *, const Entity);

#endif
