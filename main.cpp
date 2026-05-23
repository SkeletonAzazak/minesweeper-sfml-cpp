#include <cmath>
#include <algorithm>

#include <SFML/Graphics.hpp>

#include "Cell.h"
#include "DrawText.h"
#include "Field.h"
#include "GetCell.h"
#include "Global.h"

template <typename T>
const T& clamp_value(const T& value, const T& low, const T& high)
{
    return std::max(low, std::min(value, high));
}

int main()
{
    sf::RenderWindow window(
        sf::VideoMode(
            sf::Vector2u(
                static_cast<unsigned int>(CELL_SIZE * COLUMNS * SCREEN_RESIZE),
                static_cast<unsigned int>(SCREEN_RESIZE * (FONT_HEIGHT + CELL_SIZE * ROWS))
            )
        ),
        "Minesweeper",
        sf::Style::Close
    );

    window.setView(
        sf::View(
            sf::FloatRect(
                sf::Vector2f(0.f, 0.f),
                sf::Vector2f(
                    static_cast<float>(CELL_SIZE * COLUMNS),
                    static_cast<float>(FONT_HEIGHT + CELL_SIZE * ROWS)
                )
            )
        )
    );

    Field field;

    while (window.isOpen())
    {
        unsigned char mouse_cell_x = clamp_value(
            static_cast<int>(
                std::floor(
                    sf::Mouse::getPosition(window).x /
                    static_cast<float>(CELL_SIZE * SCREEN_RESIZE)
                )
            ),
            0,
            COLUMNS - 1
        );

        unsigned char mouse_cell_y = clamp_value(
            static_cast<int>(
                std::floor(
                    sf::Mouse::getPosition(window).y /
                    static_cast<float>(CELL_SIZE * SCREEN_RESIZE)
                )
            ),
            0,
            ROWS - 1
        );

        while (const auto event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }

            if (const auto* keyReleased = event->getIf<sf::Event::KeyReleased>())
            {
                if (keyReleased->code == sf::Keyboard::Key::Enter)
                {
                    field.restart();
                }
            }

            if (const auto* mouseButtonReleased = event->getIf<sf::Event::MouseButtonReleased>())
            {
                if (mouseButtonReleased->button == sf::Mouse::Button::Left)
                {
                    field.open_cell(mouse_cell_x, mouse_cell_y);
                }

                if (mouseButtonReleased->button == sf::Mouse::Button::Right)
                {
                    field.flag_cell(mouse_cell_x, mouse_cell_y);
                }
            }
        }

        if (
            sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) ||
            sf::Mouse::isButtonPressed(sf::Mouse::Button::Right)
        )
        {
            field.set_mouse_state(2, mouse_cell_x, mouse_cell_y);
        }
        else
        {
            field.set_mouse_state(1, mouse_cell_x, mouse_cell_y);
        }

        window.clear(sf::Color::Black);

        field.draw(window);

        if (1 == field.effect_over())
        {
            if (1 == field.get_game_over())
            {
                draw_text(
                    1,
                    static_cast<unsigned short>(
                        std::round(0.5f * (CELL_SIZE * COLUMNS - 8 * FONT_WIDTH))
                    ),
                    static_cast<unsigned short>(
                        std::round(0.5f * (CELL_SIZE * ROWS - FONT_HEIGHT))
                    ),
                    "VICTORY!",
                    window
                );
            }
            else if (-1 == field.get_game_over())
            {
                draw_text(
                    1,
                    static_cast<unsigned short>(
                        std::round(0.5f * (CELL_SIZE * COLUMNS - 4 * FONT_WIDTH))
                    ),
                    static_cast<unsigned short>(
                        std::round(0.5f * (CELL_SIZE * ROWS - 2 * FONT_HEIGHT))
                    ),
                    "GAME\nOVER",
                    window
                );
            }
        }

        draw_text(
            0,
            0,
            CELL_SIZE * ROWS,
            "Mines:" + std::to_string(MINES - field.get_flags()),
            window
        );

        window.display();
    }

    return 0;
}