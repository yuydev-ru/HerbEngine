#pragma once

#include <utility>
#include <iostream>

class Button
{
public:
    Button (sf::Vector2f pos, const std::unordered_map<std::string, bool>& imageStates, const std::string& text,
            const std::string& fontPath, sf::Color color, float textSize)
    {
        this->normalBtn.loadFromFile("assets/button/normal.png");
        this->hoveredBtn.loadFromFile("assets/button/hovered.png");
        this->clickedBtn.loadFromFile("assets/button/clicked.png");

        this->pos = pos;
        this->text = text;
        this->fontPath = fontPath;
        this->color = color;
        this->textSize = textSize;
        this->imageStates = imageStates;
    }

    void DrawSprites (sf::RenderWindow& window);
    void isHovered (sf::RenderWindow& window, sf::Sprite sprite);
    void isClicked (sf::RenderWindow& window, sf::Sprite sprite);

private:
    sf::Texture normalBtn, hoveredBtn, clickedBtn;
    sf::Sprite sprite;
    sf::Vector2f pos;
    std::string text;
    std::string fontPath;
    sf::Color color;
    float textSize;
    static void* func();
    std::unordered_map<std::string, bool> imageStates;
    std::unordered_map<std::string, sf::Texture> images;
};

void
Button::DrawSprites (sf::RenderWindow& window)
{
    sf::Sprite btnSprite;
    btnSprite.setTexture(this->normalBtn);
    btnSprite.setPosition(pos);
    window.draw(btnSprite);
}

void
Button::isHovered(sf::RenderWindow& window, sf::Sprite sprite)
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

void
Button::isClicked(sf::RenderWindow& window, sf::Sprite sprite)
{
    sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
    if (sprite.getGlobalBounds().contains(mousePos) && sf::Mouse::isButtonPressed(sf::Mouse::Left))
    {
        sf::Sprite clickedBtnSprite;
        clickedBtnSprite.setTexture(hoveredBtn);
        clickedBtnSprite.setPosition(pos);
        window.draw(clickedBtnSprite);
        func();
    }
}

void*
Button::func() {
    std::cout << "LOX";
}
