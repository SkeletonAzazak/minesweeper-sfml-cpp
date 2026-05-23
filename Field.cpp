#include <cmath>
#include <random>
#include <string>
#include <chrono>

#include <SFML/Graphics.hpp>

#include "Cell.h"
#include "Field.h"
#include "GetCell.h"
#include "Global.h"

Field::Field() :
    first_click(0),
    game_over(1),
    random_engine(
        static_cast<unsigned>(
            std::chrono::steady_clock::now().time_since_epoch().count()
        )
    )
{
    // Додаємо всі клітинки до вектора ігрового поля
    for (unsigned char a = 0; a < ROWS; a++)
    {
        for (unsigned char b = 0; b < COLUMNS; b++)
        {
            cells.push_back(Cell(b, a));
        }
    }

    // Запускаємо початковий стан гри
    restart();
}

bool Field::effect_over()
{
    // Перевіряємо, чи завершився ефект для всіх клітинок
    for (Cell& cell : cells)
    {
        if (0 < cell.get_effect_timer())
        {
            return 0;
        }
    }

    return 1;
}

char Field::get_game_over()
{
    return game_over;
}

unsigned short Field::get_flags()
{
    // Підраховуємо кількість встановлених прапорців
    unsigned short total_flags = 0;

    for (Cell& cell : cells)
    {
        total_flags += cell.get_is_flagged();
    }

    return total_flags;
}

void Field::draw(sf::RenderWindow& i_window)
{
    // Форма однієї клітинки
    sf::RectangleShape cell_shape(
        sf::Vector2f(
            static_cast<float>(CELL_SIZE - 1),
            static_cast<float>(CELL_SIZE - 1)
        )
    );

    // Текстура з іконками: прапорець і числа
    sf::Texture icons_texture;

    if (!icons_texture.loadFromFile(
        "Resources/Images/Icons" + std::to_string(static_cast<int>(CELL_SIZE)) + ".png"
    ))
    {
        return;
    }

    sf::Sprite icon_sprite(icons_texture);

    // Малюємо всі клітинки поля
    for (unsigned char a = 0; a < COLUMNS; a++)
    {
        for (unsigned char b = 0; b < ROWS; b++)
        {
            cell_shape.setPosition(
                sf::Vector2f(
                    static_cast<float>(CELL_SIZE * a),
                    static_cast<float>(CELL_SIZE * b)
                )
            );

            // Якщо клітинка відкрита
            if (1 == get_cell(a, b, cells)->get_is_open())
            {
                unsigned char mines_around =
                    get_cell(a, b, cells)->get_mines_around();

                cell_shape.setFillColor(sf::Color(146, 182, 255));

                i_window.draw(cell_shape);

                // Якщо навколо клітинки є міни, малюємо число
                if (0 < mines_around)
                {
                    icon_sprite.setPosition(
                        sf::Vector2f(
                            static_cast<float>(CELL_SIZE * a),
                            static_cast<float>(CELL_SIZE * b)
                        )
                    );

                    icon_sprite.setTextureRect(
                        sf::IntRect(
                            sf::Vector2i(
                                static_cast<int>(CELL_SIZE * mines_around),
                                0
                            ),
                            sf::Vector2i(
                                static_cast<int>(CELL_SIZE),
                                static_cast<int>(CELL_SIZE)
                            )
                        )
                    );

                    i_window.draw(icon_sprite);
                }
            }
            else
            {
                // Якщо клітинка закрита
                cell_shape.setFillColor(sf::Color::Black);

                // Підсвічування клітинки при наведенні або натисканні миші
                if (0 == game_over)
                {
                    if (1 == get_cell(a, b, cells)->get_mouse_state())
                    {
                        cell_shape.setFillColor(sf::Color::Magenta);
                    }
                    else if (2 == get_cell(a, b, cells)->get_mouse_state())
                    {
                        cell_shape.setFillColor(sf::Color::Red);
                    }
                }

                i_window.draw(cell_shape);

                // Якщо встановлено прапорець, малюємо його
                if (1 == get_cell(a, b, cells)->get_is_flagged())
                {
                    icon_sprite.setPosition(
                        sf::Vector2f(
                            static_cast<float>(CELL_SIZE * a),
                            static_cast<float>(CELL_SIZE * b)
                        )
                    );

                    icon_sprite.setTextureRect(
                        sf::IntRect(
                            sf::Vector2i(0, 0),
                            sf::Vector2i(
                                static_cast<int>(CELL_SIZE),
                                static_cast<int>(CELL_SIZE)
                            )
                        )
                    );

                    i_window.draw(icon_sprite);
                }
            }

            // Скидаємо стан миші для клітинки
            get_cell(a, b, cells)->set_mouse_state(0);

            // Малювання ефекту після перемоги або програшу
            if (
                0 != game_over &&
                EFFECT_DURATION > get_cell(a, b, cells)->get_effect_timer()
            )
            {
                unsigned char effect_size = static_cast<unsigned char>(
                    2 * round(
                        0.5f * CELL_SIZE *
                        (
                            (EFFECT_DURATION -
                            get_cell(a, b, cells)->get_effect_timer()) /
                            static_cast<float>(EFFECT_DURATION)
                        )
                    )
                );

                std::uniform_int_distribution<unsigned short>
                    effect_duration_distribution(1, EFFECT_DURATION - 1);

                cell_shape.setPosition(
                    sf::Vector2f(
                        static_cast<float>(
                            floor(CELL_SIZE * (0.5f + a) - 0.5f * effect_size)
                        ),
                        static_cast<float>(
                            floor(CELL_SIZE * (0.5f + b) - 0.5f * effect_size)
                        )
                    )
                );

                cell_shape.setSize(
                    sf::Vector2f(
                        static_cast<float>(effect_size),
                        static_cast<float>(effect_size)
                    )
                );

                // Колір ефекту залежить від результату гри
                if (-1 == game_over)
                {
                    cell_shape.setFillColor(sf::Color::Red);
                }
                else
                {
                    cell_shape.setFillColor(sf::Color::Black);
                }

                i_window.draw(cell_shape);

                // Повертаємо звичайний розмір клітинки
                cell_shape.setSize(
                    sf::Vector2f(
                        static_cast<float>(CELL_SIZE - 1),
                        static_cast<float>(CELL_SIZE - 1)
                    )
                );

                // Якщо таймер ефекту завершився, запускаємо ефект для сусідніх клітинок
                if (1 == get_cell(a, b, cells)->update_effect_timer())
                {
                    if (
                        0 < a &&
                        EFFECT_DURATION ==
                        get_cell(a - 1, b, cells)->get_effect_timer()
                    )
                    {
                        get_cell(a - 1, b, cells)->set_effect_timer(
                            static_cast<unsigned char>(
                                effect_duration_distribution(random_engine)
                            )
                        );
                    }

                    if (
                        0 < b &&
                        EFFECT_DURATION ==
                        get_cell(a, b - 1, cells)->get_effect_timer()
                    )
                    {
                        get_cell(a, b - 1, cells)->set_effect_timer(
                            static_cast<unsigned char>(
                                effect_duration_distribution(random_engine)
                            )
                        );
                    }

                    if (
                        COLUMNS > 1 + a &&
                        EFFECT_DURATION ==
                        get_cell(1 + a, b, cells)->get_effect_timer()
                    )
                    {
                        get_cell(1 + a, b, cells)->set_effect_timer(
                            static_cast<unsigned char>(
                                effect_duration_distribution(random_engine)
                            )
                        );
                    }

                    if (
                        ROWS > 1 + b &&
                        EFFECT_DURATION ==
                        get_cell(a, 1 + b, cells)->get_effect_timer()
                    )
                    {
                        get_cell(a, 1 + b, cells)->set_effect_timer(
                            static_cast<unsigned char>(
                                effect_duration_distribution(random_engine)
                            )
                        );
                    }
                }
            }
        }
    }
}

void Field::flag_cell(unsigned char i_x, unsigned char i_y)
{
    // Не дозволяємо ставити прапорець після завершення гри
    if (0 == game_over)
    {
        get_cell(i_x, i_y, cells)->flag();
    }
}

void Field::open_cell(unsigned char i_x, unsigned char i_y)
{
    // Якщо це перший хід, розміщуємо міни
    if (0 == first_click)
    {
        std::uniform_int_distribution<unsigned short>
            x_distribution(0, COLUMNS - 1);

        std::uniform_int_distribution<unsigned short>
            y_distribution(0, ROWS - 1);

        first_click = 1;

        for (unsigned short a = 0; a < MINES; a++)
        {
            unsigned char mine_x =
                static_cast<unsigned char>(x_distribution(random_engine));

            unsigned char mine_y =
                static_cast<unsigned char>(y_distribution(random_engine));

            // Не ставимо міну в клітинку першого ходу
            // і не ставимо міну туди, де вона вже є
            if (
                1 == get_cell(mine_x, mine_y, cells)->get_is_mine() ||
                (i_x == mine_x && i_y == mine_y)
            )
            {
                a--;
            }
            else
            {
                get_cell(mine_x, mine_y, cells)->set_mine();
            }
        }

        // Після розміщення мін підраховуємо кількість мін навколо кожної клітинки
        for (Cell& cell : cells)
        {
            cell.count_mines_around(cells);
        }
    }

    // Не відкриваємо клітинку, якщо гра завершена або клітинка позначена прапорцем
    if (
        0 == game_over &&
        0 == get_cell(i_x, i_y, cells)->get_is_flagged()
    )
    {
        if (1 == get_cell(i_x, i_y, cells)->open(cells))
        {
            // Якщо відкрили міну — програш
            game_over = -1;
        }
        else
        {
            unsigned short total_closed_cells = 0;

            // Рахуємо кількість закритих клітинок
            for (Cell& cell : cells)
            {
                total_closed_cells += 1 - cell.get_is_open();
            }

            // Якщо закритими залишились тільки міни — перемога
            if (MINES == total_closed_cells)
            {
                game_over = 1;

                get_cell(i_x, i_y, cells)->set_effect_timer(EFFECT_DURATION - 1);
            }
        }
    }
}

void Field::restart()
{
    // Перезапуск можливий тільки після завершення гри
    if (0 != game_over)
    {
        first_click = 0;
        game_over = 0;

        for (Cell& cell : cells)
        {
            cell.reset();
        }
    }
}

void Field::set_mouse_state(
    unsigned char i_mouse_state,
    unsigned char i_x,
    unsigned char i_y
)
{
    get_cell(i_x, i_y, cells)->set_mouse_state(i_mouse_state);
}