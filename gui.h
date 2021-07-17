#pragma once

#include <utility>
#include <iostream>

class Button
{
public:
    Button(const sf::Vector2f& pos, const std::unordered_map<std::string, std::string>& imageStates,
           const std::string& text, const std::string& fontPath, const sf::Color fontColor,
           unsigned textSize, void func())
    {
        this->normalBtn.loadFromFile(imageStates.at("normal"));
        this->hoveredBtn.loadFromFile(imageStates.at("hovered"));
        this->clickedBtn.loadFromFile(imageStates.at("clicked"));

        this->pos = pos;
        this->func = *func;

        sf::Font font;
        font.loadFromFile(fontPath);
        this->buttonText.setFont(font);
        this->buttonText.setString(text);
        this->buttonText.setFillColor(fontColor);
        this->buttonText.setCharacterSize(textSize);
        this->buttonText.setStyle(sf::Text::Regular);

        this->states = { {"after_click", false}
                       , {"hovered", false}
                       , {"clicked", false} };
    }

    void drawButton(sf::RenderWindow& window);
    void updateButton(sf::RenderWindow& window, sf::Event&);

private:
    sf::Texture normalBtn, hoveredBtn, clickedBtn;
    sf::Sprite sprite;
    sf::Vector2f pos;
    sf::Text buttonText;
    std::unordered_map<std::string, bool> states;
    void (*func)();
};

void
Button::drawButton(sf::RenderWindow& window)
{
    if (states["clicked"])
    {
        sprite.setTexture(clickedBtn);
        states["clicked"] = false;
        states["after_click"] = true;
    }
    else if (states["after_click"])
    {
        if (states["hovered"])
        {
            sprite.setTexture(clickedBtn);
        }
        else
        {
            states["after_click"] = false;
        }
    }
    else if (states["hovered"])
    {
        sprite.setTexture(hoveredBtn);
    }
    else
    {
        sprite.setTexture(normalBtn);
    }

    sprite.setPosition(pos);
    window.draw(sprite);
    window.draw(buttonText);
}

void
Button::updateButton(sf::RenderWindow& window, sf::Event& event)
{
    sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
    states["hovered"] = sprite.getGlobalBounds().contains(mousePos);
    if (event.type == sf::Event::MouseButtonPressed)
    {
        if (event.mouseButton.button == sf::Mouse::Left)
        {
            if (states["hovered"])
            {
                states["clicked"] = true;
            }
        }
    }
    else if (event.type == sf::Event::MouseButtonReleased)
    {
        if (event.mouseButton.button == sf::Mouse::Left)
        {
            if (states["hovered"] && states["after_click"])
            {
                states["after_click"] = false;
                func();
            }
        }
    }
}

void
func() {
    puts("Click!");
}