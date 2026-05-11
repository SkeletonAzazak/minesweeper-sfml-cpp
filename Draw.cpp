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
    // Вирівняємо текст
    short character_x = i_x;
    short character_y = i_y;

    unsigned char character_width;

    sf::Sprite character_sprite;
    sf::Texture font_texture;

    font_texture.loadFromFile("D:/source/repos/Font.png");

    // Ми обчислимо ширину символу на основі розміру зображення шрифту.
    // 96, тому що на зображенні 96 символів
    character_width = font_texture.getSize().x / 96;

    character_sprite.setTexture(font_texture);

    // Ми можемо використовувати це, щоб змінити колір тексту
    if (1 == i_black)
    {
        character_sprite.setColor(sf::Color(0, 0, 0));
    }

    for (const char a : i_text)
    {
        if ('\n' == a)
        {
            // Після кожного нового рядка ми ставимо збільшення y-координати
            // та скидання x-координати
            character_x = i_x;
            character_y += font_texture.getSize().y;

            continue;
        }

        // Змінюємо позицію наступного символу
        character_sprite.setPosition(character_x, character_y);

        // Вибираємо символ із зображення шрифту
        character_sprite.setTextureRect(
            sf::IntRect(
                character_width * (a - 32),
                0,
                character_width,
                font_texture.getSize().y
            )
        );

        // Збільшуємо координату x
        character_x += character_width;

        // Робимо, щоб відображало вікно
        i_window.draw(character_sprite);
    }
}