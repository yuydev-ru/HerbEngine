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

struct Animation
{
    std::string name;
    std::string assetPath;
    sf::Vector2<int> gridSize;
    int numberOfFrames = 0;
    float timeout = 0;

    sf::Image image;
    sf::Texture texture;
    sf::Sprite sprite;
    std::vector<sf::IntRect> frames;
};

struct AnimationController : Component
{
    std::vector<Animation *> animations;

    Animation *currentAnimation {};
    int currentFrame = 0;
    float timeout = 0.0f;

    void
    setAnimation(const std::string &name);

    static Component *
    deserialize(Parser &parser)
    {
        auto controller = new AnimationController;

        // TODO(andrew): Сделать нормальную итерацию по парсеру.
        auto outerAnimParser = Parser(parser.data["animations"]);
        for (outerAnimParser.iterator = outerAnimParser.data.begin(); outerAnimParser.iterator != outerAnimParser.data.end(); ++outerAnimParser.iterator) {
            auto animParser = Parser(*outerAnimParser.iterator);

            // TODO(andrew): Найти способ делать move в вектор, вместо хранения указателей.
            auto anim = new Animation;
            anim->name = animParser.parseElement<std::string>("name");
            anim->assetPath = animParser.parseElement<std::string>("assetPath");
            anim->gridSize = animParser.parseVector2<int>("gridSize");
            anim->numberOfFrames = animParser.parseElement<int>("numberOfFrames");
            auto fps = animParser.parseElement<float>("fps");
            anim->timeout = 1.0f / fps;

            anim->image.loadFromFile(anim->assetPath);
            anim->texture.loadFromImage(anim->image);
            anim->sprite.setTexture(anim->texture);

            sf::Vector2<int> frameSize = {(int) anim->image.getSize().x / anim->gridSize.x,
                                          (int) anim->image.getSize().y / anim->gridSize.y};

            int nFrames = 0;
            for (int j = 0; j < anim->gridSize.y && nFrames <= anim->numberOfFrames; j++) {
                for (int i = 0; i < anim->gridSize.x && nFrames <= anim->numberOfFrames; i++) {
                    sf::IntRect rect = {i * frameSize.x, j * frameSize.y, frameSize.x, frameSize.y};
                    anim->frames.push_back(rect);
                    nFrames++;
                }
            }

            controller->animations.push_back(anim);
        }

        auto defaultAnimation = parser.parseElement<std::string>("defaultAnimation");
        controller->setAnimation(defaultAnimation);

        return controller;
    }
};

void
render(GameState *, Storage *, Entity);

void
updateAnimation(GameState *, Storage *, Entity);

#endif