#include <utility>

#pragma once

class Button
{
public:
    Button (sf::Vector2f pos, const std::unordered_map<std::string, bool>& imageStates, const std::string& text,
            const std::string& fontPath, sf::Color color, float textSize, void* func)
    {
        this->pos = pos;
        this->text = text;
        this->fontPath = fontPath;
        this->color = color;
        this->textSize = textSize;
        this->func = func;
        this->imageStates = imageStates;
    }

private:
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