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
    Text(const sf::Vector2f &, const std::string &, unsigned = 14, sf::Color = sf::Color::White, const std::string& = "assets/fonts/Neucha-Regular.ttf");
    ~Text() { delete font; }
    void drawText(sf::RenderWindow&);

private:
    sf::Text text;
    sf::Font *font;
    sf::Vector2f pos;
    sf::FloatRect textRect;
};


class Button : Widget {
public:
    Button(const sf::Vector2f &pos, const std::unordered_map<std::string, std::string> &imageStates,
           const std::string &text, const std::string &fontPath, const sf::Color fillColor,
           unsigned textSize, void func()) {
        this->normalBtn.loadFromFile(imageStates.at("normal"));
        this->hoveredBtn.loadFromFile(imageStates.at("hovered"));
        this->clickedBtn.loadFromFile(imageStates.at("clicked"));

        this->pos = pos;
        this->func = *func;

        this->font = new sf::Font;
        font->loadFromFile(fontPath);
        this->buttonText.setFont(*font);

        this->buttonText.setString(text);
        this->buttonText.setFillColor(fillColor);
        this->buttonText.setCharacterSize(textSize);
        this->buttonText.setStyle(sf::Text::Regular);
        this->buttonText.setPosition(50.f, 50.f);

        this->states = {{"after_click", false},
                        {"hovered",     false},
                        {"clicked",     false}};
    }

    ~Button() { delete font; }

    void
    drawButton(sf::RenderWindow &window) {
        if (states["clicked"]) {
            sprite.setTexture(clickedBtn);
            states["clicked"] = false;
            states["after_click"] = true;
        } else if (states["after_click"]) {
            if (states["hovered"]) {
                sprite.setTexture(clickedBtn);
            } else {
                states["after_click"] = false;
            }
        } else if (states["hovered"]) {
            sprite.setTexture(hoveredBtn);
        } else {
            sprite.setTexture(normalBtn);
        }

        window.draw(sprite);
        window.draw(buttonText);
    }

    void
    updateButton(sf::RenderWindow &window, sf::Event &event) {
        sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
        states["hovered"] = sprite.getGlobalBounds().contains(mousePos);
        if (event.type == sf::Event::MouseButtonPressed) {
            if (event.mouseButton.button == sf::Mouse::Left) {
                if (states["hovered"]) {
                    states["clicked"] = true;
                }
            }
        } else if (event.type == sf::Event::MouseButtonReleased) {
            if (event.mouseButton.button == sf::Mouse::Left) {
                if (states["hovered"] && states["after_click"]) {
                    states["after_click"] = false;
                    func();
                }
            }
        }
    }

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