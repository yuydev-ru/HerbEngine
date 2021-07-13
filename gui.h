#include <utility>
#include <iostream>

#pragma once

class Button
{
public:
    Button (sf::Vector2f pos, const std::unordered_map<std::string, bool>& imageStates, const std::string& text,
            const std::string& fontPath, sf::Color color, float textSize, void* func)
    {
        sf::Texture btn, hoveredBtn, clickedBtn;
        btn.loadFromFile("assets/button/normal.png");
        std::cout << btn->getSize().x << '\n';
        std::cout << btn->getSize().y<< '\n';
        btn.setSmooth(true);
        hoveredBtn.loadFromFile("assets/button/hovered.png"); hoveredBtn.setSmooth(true);
        clickedBtn.loadFromFile("assets/button/clicked.png"); clickedBtn.setSmooth(true);

        this->pos = pos;
        this->text = text;
        this->fontPath = fontPath;
        this->color = color;
        this->textSize = textSize;
        this->func = func;
        this->imageStates = imageStates;
    }
    void DrawSprites (sf::RenderWindow& window);
    void isHovered (sf::RenderWindow& window, sf::Sprite sprite);
    void isClicked (sf::RenderWindow& window, sf::Sprite sprite);
private:
    sf::Texture btn, hoveredBtn, clickedBtn;
    sf::Sprite sprite;
    sf::Vector2f pos;
    std::string text;
    std::string fontPath;
    sf::Color color;
    float textSize;
    void* func;
    std::unordered_map<std::string, bool> imageStates;
    std::unordered_map<std::string, sf::Texture> images;
};

void Button::DrawSprites (sf::RenderWindow& window)
{
    sf::Sprite btnSprite;
    btnSprite.setTexture(btn);
    btnSprite.setPosition(pos);
    window.draw(btnSprite);
}

void Button::isHovered (sf::RenderWindow& window, sf::Sprite sprite)
{
    sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
    if (sprite.getGlobalBounds().contains(mousePos))
    {
        sf::Sprite hoveredBtnSprite;
        hoveredBtnSprite.setTexture(hoveredBtn);
        hoveredBtnSprite.setPosition(pos);
        window.draw(hoveredBtnSprite);
    }
}

void Button::isClicked (sf::RenderWindow& window, sf::Sprite sprite)
{
    sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
    if (sprite.getGlobalBounds().contains(mousePos) && sf::Mouse::isButtonPressed(sf::Mouse::Left))
    {
        sf::Sprite clickedBtnSprite;
        clickedBtnSprite.setTexture(hoveredBtn);
        clickedBtnSprite.setPosition(pos);
        window.draw(clickedBtnSprite);
        reinterpret_cast<void*>(func);
    }
}