#include <string>

#include <SFML/Graphics.hpp>

#include "DrawText.h"

void draw_text(
    bool i_black,
    unsigned short i_x,
    unsigned short i_y,
    const std::string& i_text,
    sf::RenderWindow& i_window
)
{
    short character_x = i_x;
    short character_y = i_y;

    unsigned char character_width;

    sf::Texture font_texture;

    if (!font_texture.loadFromFile("Resources/Images/Font.png"))
    {
        return;
    }

    character_width = static_cast<unsigned char>(font_texture.getSize().x / 96);

    sf::Sprite character_sprite(font_texture);

    if (1 == i_black)
    {
        character_sprite.setColor(sf::Color(0, 0, 0));
    }

    for (const char a : i_text)
    {
        if ('\n' == a)
        {
            character_x = i_x;
            character_y += static_cast<short>(font_texture.getSize().y);

            continue;
        }

        character_sprite.setPosition(
            sf::Vector2f(
                static_cast<float>(character_x),
                static_cast<float>(character_y)
            )
        );

        character_sprite.setTextureRect(
            sf::IntRect(
                sf::Vector2i(
                    static_cast<int>(character_width * (a - 32)),
                    0
                ),
                sf::Vector2i(
                    static_cast<int>(character_width),
                    static_cast<int>(font_texture.getSize().y)
                )
            )
        );

        character_x += character_width;

        i_window.draw(character_sprite);
    }
}