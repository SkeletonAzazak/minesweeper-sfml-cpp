#include <chrono>
#include <random>
#include <cmath>
#include <algorithm>

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include "Cell.h"
#include "DrawText.h"
#include "Field.h"
#include "GetCell.h"
#include "Global.h"

// Реалізація функції clamp
template <typename T>
const T& clamp(const T& value, const T& low, const T& high)
{
    return std::max(low, std::min(value, high));
}

int main()
{
    // Використовується, щоб зробити гру незалежною від частоти кадрів
    unsigned lag = 0;

    // Подібно до затримки, використовується, щоб зробити гру незалежною від частоти кадрів
    std::chrono::time_point<std::chrono::steady_clock> previous_time;

    // Зберігає події
    sf::Event event;

    // SFML-вікно
    sf::RenderWindow window(
        sf::VideoMode(
            CELL_SIZE * COLUMNS * SCREEN_RESIZE,
            SCREEN_RESIZE * (FONT_HEIGHT + CELL_SIZE * ROWS)
        ),
        "Minesweeper",
        sf::Style::Close
    );

    // Тут ми змінюємо розмір вікна
    window.setView(sf::View(sf::FloatRect(
        0,
        0,
        CELL_SIZE * COLUMNS,
        FONT_HEIGHT + CELL_SIZE * ROWS
    )));

    // Об’єкт поля під назвою field
    Field field;

    // Звук вибуху, коли програю
    sf::SoundBuffer explosionBuffer;

    if (!explosionBuffer.loadFromFile("D:/source/repos/explosion.mp3"))
    {
        return -1;
    }

    // Створюю звук
    sf::Sound explosionSound;
    explosionSound.setBuffer(explosionBuffer);

    bool explosion_played = false;

    // Отримую поточний час та зберігаю його в змінній
    previous_time = std::chrono::steady_clock::now();

    while (window.isOpen())
    {
        // Тут ми обчислюємо затримку
        unsigned delta_time =
            std::chrono::duration_cast<std::chrono::microseconds>(
                std::chrono::steady_clock::now() - previous_time
            ).count();

        lag += delta_time;
        previous_time += std::chrono::microseconds(delta_time);

        // Поки затримка перевищує максимально допустиму тривалість кадру
        while (FRAME_DURATION <= lag)
        {
            // Отримуємо координати комірки під курсором
            unsigned char mouse_cell_x = clamp(
                static_cast<int>(floor(
                    sf::Mouse::getPosition(window).x /
                    static_cast<float>(CELL_SIZE * SCREEN_RESIZE)
                )),
                0,
                COLUMNS - 1
            );

            unsigned char mouse_cell_y = clamp(
                static_cast<int>(floor(
                    sf::Mouse::getPosition(window).y /
                    static_cast<float>(CELL_SIZE * SCREEN_RESIZE)
                )),
                0,
                ROWS - 1
            );

            lag -= FRAME_DURATION;

            while (window.pollEvent(event))
            {
                switch (event.type)
                {
                    // Якщо користувач натиснув кнопку закриття вікна
                    case sf::Event::Closed:
                    {
                        window.close();
                        break;
                    }

                    case sf::Event::KeyReleased:
                    {
                        switch (event.key.code)
                        {
                            case sf::Keyboard::Enter:
                            {
                                // Ми перезапускаємо гру, якщо була відпущена клавіша Enter
                                field.restart();
                                explosion_played = false;
                                break;
                            }

                            default:
                                break;
                        }

                        break;
                    }

                    case sf::Event::MouseButtonReleased:
                    {
                        switch (event.mouseButton.button)
                        {
                            // Якщо натискаємо ліву кнопку миші
                            case sf::Mouse::Left:
                            {
                                // Відкриваємо комірку під курсором
                                field.open_cell(mouse_cell_x, mouse_cell_y);
                                break;
                            }

                            // Якщо натискаємо праву кнопку миші
                            case sf::Mouse::Right:
                            {
                                // Позначаємо клітинку прапором під курсором
                                field.flag_cell(mouse_cell_x, mouse_cell_y);
                                break;
                            }

                            default:
                                break;
                        }

                        break;
                    }

                    default:
                        break;
                }
            }

            // Якщо в даний момент натиснуто будь-яку клітинку
            if (
                sf::Mouse::isButtonPressed(sf::Mouse::Left) ||
                sf::Mouse::isButtonPressed(sf::Mouse::Right)
            )
            {
                // Ми також змінюємо стан комірки миші
                field.set_mouse_state(2, mouse_cell_x, mouse_cell_y);
            }
            else
            {
                field.set_mouse_state(1, mouse_cell_x, mouse_cell_y);
            }

            // Якщо гра програна
            if (-1 == field.get_game_over() && !explosion_played)
            {
                // Відтворюємо звук вибуху
                explosionSound.play();
                explosion_played = true;
            }

            // Малюємо вікно
            if (FRAME_DURATION > lag)
            {
                // Спочатку очищаємо вікно
                window.clear();

                // Потім малюємо ігрове поле
                field.draw(window);

                // Тут ми показуємо текст, який повідомляє гравця про те,
                // що він програв або виграв гру
                if (1 == field.effect_over())
                {
                    if (1 == field.get_game_over())
                    {
                        draw_text(
                            1,
                            static_cast<unsigned short>(
                                round(0.5f * (CELL_SIZE * COLUMNS - 8 * FONT_WIDTH))
                            ),
                            static_cast<unsigned short>(
                                round(0.5f * (CELL_SIZE * ROWS - FONT_HEIGHT))
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
                                round(0.5f * (CELL_SIZE * COLUMNS - 4 * FONT_WIDTH))
                            ),
                            static_cast<unsigned short>(
                                round(0.5f * (CELL_SIZE * ROWS - 2 * FONT_HEIGHT))
                            ),
                            "GAME\nOVER",
                            window
                        );
                    }
                }

                // Показуємо скільки мін залишилось
                draw_text(
                    0,
                    0,
                    CELL_SIZE * ROWS,
                    "Mines:" + std::to_string(MINES - field.get_flags()),
                    window
                );

                // Відображаємо усе
                window.display();
            }
        }
    }

    return 0;
}