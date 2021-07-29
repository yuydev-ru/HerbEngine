#ifndef ENGINE_GUI_H
#define ENGINE_GUI_H

#include <utility>
#include <iostream>
#include <unordered_map>
#include <SFML/Graphics.hpp>

class Widget
{
public:
    virtual void draw(sf::RenderWindow&){}
    virtual void update(sf::RenderWindow&, sf::Event&){}
};

class Text : public Widget
{
public:
    Text( const sf::Vector2f &, const std::string &, unsigned = 14, sf::Color = sf::Color::White
        , const std::string& = "assets/fonts/Neucha-Regular.ttf" );
    ~Text();

    void draw(sf::RenderWindow&) override;

private:
    sf::Text text;
    sf::Font *font;
    sf::Vector2f pos;
    sf::FloatRect textRect;
};

class Button : public Widget
{
public:
    Button( const std::string&, const sf::Vector2f&, const std::unordered_map<std::string, std::string>&
          , const std::string& = "", unsigned = 14, const std::vector<int>& = {255, 255, 255, 255}
          , const std::string& = "assets/fonts/Neucha-Regular.ttf" );
    ~Button();

    void draw(sf::RenderWindow&) override;
    void update(sf::RenderWindow&, sf::Event&) override;

private:
    sf::Texture normalBtn, hoveredBtn, clickedBtn;
    sf::Sprite sprite;
    sf::Font *font;
    sf::Vector2f pos;
    sf::Text buttonText;
    std::unordered_map<std::string, bool> states;
    std::string func;
};

#endif