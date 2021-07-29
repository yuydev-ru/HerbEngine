#include "gui.h"

Text::Text( const sf::Vector2f &pos, const std::string &str, const unsigned characterSize
          , const sf::Color fillColor
          , const std::string &fontPath )
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

Text::~Text()
{
    delete font;
}

void
Text::draw(sf::RenderWindow &window)
{
    text.setPosition(pos);
    window.draw(text);
}

Button::Button( const std::string& func, const sf::Vector2f &pos, const std::unordered_map<std::string, std::string> &imageStates
              , const std::string &text, unsigned textSize, const std::vector<int>& fillColor, const std::string &fontPath ) {
    this->normalBtn.loadFromFile(imageStates.at("normal"));
    this->hoveredBtn.loadFromFile(imageStates.at("hovered"));
    this->clickedBtn.loadFromFile(imageStates.at("clicked"));

    this->pos = pos;
    this->func = func;

    this->font = new sf::Font;
    font->loadFromFile(fontPath);
    this->buttonText.setFont(*font);

    this->buttonText.setString(text);
    this->buttonText.setFillColor(sf::Color(fillColor[0], fillColor[1], fillColor[2], fillColor[3]));
    this->buttonText.setCharacterSize(textSize);
    this->buttonText.setStyle(sf::Text::Regular);
    this->buttonText.setPosition(50.f, 50.f);

    this->states = { {"after_click", false}
                   , {"hovered",     false}
                   , {"clicked",     false} };
}

Button::~Button()
{
    delete font;
}

void
Button::draw(sf::RenderWindow &window) {
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
Button::update(sf::RenderWindow &window, sf::Event &event)
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
                puts("Click!");
//                func();
            }
        }
    }
}
