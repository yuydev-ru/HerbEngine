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

        if (!parser.isNull("assetPath"))
        {
            spr->assetPath = parser.parseElement<std::string>("assetPath");

            spr->image.loadFromFile(spr->assetPath);
            spr->texture.loadFromImage(spr->image);
            spr->sprite.setTexture(spr->texture);
            sf::IntRect rect = { 0, 0
                               , (int) spr->image.getSize().x
                               , (int) spr->image.getSize().y };
            spr->sprite.setTextureRect(rect);
        }

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

struct Animation : Component
{
    std::string assetPath;
    sf::Vector2<int> gridSize;
    int numberOfFrames = 0;

    sf::Image image;
    sf::Texture texture;
    sf::Sprite sprite;

    std::vector<sf::IntRect> frames;
    int currentFrame = 0;

    float t = 0.0f;
    float timeout = 0.0f;

    static Component *
    deserialize(Parser &parser)
    {
        auto anim = new Animation;

        anim->assetPath = parser.parseElement<std::string>("assetPath");
        anim->gridSize = parser.parseVector2<int>("gridSize");
        anim->numberOfFrames = parser.parseElement<int>("numberOfFrames");
        auto fps = parser.parseElement<float>("fps");
        anim->timeout = 1.0f / fps;

        anim->image.loadFromFile(anim->assetPath);
        anim->texture.loadFromImage(anim->image);
        anim->sprite.setTexture(anim->texture);

        sf::Vector2<int> frameSize = { (int) anim->image.getSize().x / anim->gridSize.x
                                     , (int) anim->image.getSize().y / anim->gridSize.y };

        int nFrames = 0;
        for (int i = 0; i < anim->gridSize.x && nFrames <= anim->numberOfFrames; i++)
        {
            for (int j = 0; j < anim->gridSize.y && nFrames <= anim->numberOfFrames; j++)
            {
                sf::IntRect rect = {i * frameSize.x, j * frameSize.y, frameSize.x, frameSize.y};
                anim->frames.push_back(rect);
                nFrames++;
            }
        }

        return anim;
    }
};

void
render(GameState *, Storage *, Entity);

void
updateAnimation(GameState *, Storage *, Entity);

#endif
