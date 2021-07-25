#ifndef ENGINE_GUI_H
#define ENGINE_GUI_H

#include <utility>
#include <iostream>
#include <unordered_map>
#include <SFML/Graphics.hpp>

class Widget {
    // https://stackoverflow.com/questions/26208918/vector-that-can-have-3-different-data-types-c
};

class Text : Widget {
public:
    Text( const sf::Vector2f &, const std::string &, unsigned = 14, sf::Color = sf::Color::White
        , const std::string& = "assets/fonts/Neucha-Regular.ttf" );
    ~Text();

    void drawText(sf::RenderWindow&);

private:
    sf::Text text;
    sf::Font *font;
    sf::Vector2f pos;
    sf::FloatRect textRect;
};


class Button : Widget {
public:
    Button( const sf::Vector2f&, const std::unordered_map<std::string, std::string>&, const std::string&
          , const std::string&, sf::Color, unsigned, void() );
    ~Button();

    void drawButton(sf::RenderWindow&);
    void updateButton(sf::RenderWindow&, sf::Event&);

private:
    sf::Texture normalBtn, hoveredBtn, clickedBtn;
    sf::Sprite sprite;
    sf::Font *font;
    sf::Vector2f pos;
    sf::Text buttonText;
    std::unordered_map<std::string, bool> states;
    void (*func)();
};

static void
func() {
    puts("Click!");
}

#endif