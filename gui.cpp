#include "gui.h"

Text::Text( const sf::Vector2f &pos, const std::string &str, const unsigned characterSize
          , const sf::Color fillColor
          , const std::string &fontPath)
{
    this->text.setString(str);
    this->text.setCharacterSize(characterSize);
    this->text.setFillColor(fillColor);

    this->textRect = this->text.getLocalBounds();
    this->pos = pos;

    this->font = new sf::Font;
    font->loadFromFile(fontPath);
    this->text.setFont(*font);
}

void
Text::drawText(sf::RenderWindow &window) {
    text.setPosition(pos);
    window.draw(text);
}
