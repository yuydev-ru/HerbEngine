#ifndef ENGINE_RENDERING_H
#define ENGINE_RENDERING_H

#include "../base.h"
#include "../parser.h"
#include "gui.h"
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
struct Gui : Component
{
    std::vector<Widget*> widgets;

    static Component *
    deserialize(Parser &parser)
    {
        auto g = new Gui;

        for ( parser.iterator = parser.data["widgets"].begin()
            ; parser.iterator  != parser.data["widgets"].end()
            ; ++parser.iterator)
        {
            Parser widgetParser(*parser.iterator);
            std::string type = widgetParser.parseElement<std::string>("type");
            if (type == "Button")
            {
                auto func = widgetParser.parseElement<std::string>("func");
                auto pos = widgetParser.parseVector2<float>("position");
                auto text = widgetParser.parseElement<std::string>("text");
                std::unordered_map<std::string, std::string> imageStates;
                for ( widgetParser.iterator = widgetParser.data["imageStates"].begin()
                    ; widgetParser.iterator != widgetParser.data["imageStates"].end()
                    ; ++widgetParser.iterator)
                {
                    Parser imageStatesParser(*widgetParser.iterator);
                    imageStates.insert(std::make_pair(*imageStatesParser.data.begin(), *(imageStatesParser.data.rbegin())));
                }

                auto *button = new Button(func, pos, imageStates, text);
                g->widgets.push_back(button);
            }
        }

        return g;
    }
};

void
render(GameState *, Storage *, Entity);

void
renderGui(GameState *, Storage *, Entity);

void
updateGui(GameState *, Storage *, sf::Event &);

#endif
