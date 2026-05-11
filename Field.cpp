#include <cmath>
#include <random>

#include <SFML/Graphics.hpp>

#include "Cell.h"
#include "Field.h"
#include "GetCell.h"
#include "Global.h"

Field::Field() :
    first_click(0),
    game_over(1),
    random_engine(random_device())
{
    // Тут ми додаємо клітинки до вектора клітинок
    for (unsigned char a = 0; a < ROWS; a++)
    {
        for (unsigned char b = 0; b < COLUMNS; b++)
        {
            cells.push_back(Cell(b, a));
        }
    }

    // Подібно до функції скидання в класі Cell,
    // у нас є функція перезапуску, яка перезапускає гру
    restart();
}

bool Field::effect_over()
{
    // Ми перевіряємо таймер ефекту кожної комірки
    for (Cell& cell : cells)
    {
        // Якщо таймер ефекту будь-якої комірки ще не закінчився
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
    // Ми просто підраховуємо загальну кількість позначених клітинок
    unsigned short total_flags = 0;

    for (Cell& cell : cells)
    {
        total_flags += cell.get_is_flagged();
    }

    // І повертаємо результат
    return total_flags;
}

void Field::draw(sf::RenderWindow& i_window)
{
    // Ми використаємо це для малювання клітинок
    sf::RectangleShape cell_shape(
        sf::Vector2f(CELL_SIZE - 1, CELL_SIZE - 1)
    );

    // Мені потрібно створити 2 об’єкти, щоб намалювати зображення
    sf::Sprite icon_sprite;
    sf::Texture icons_texture;

    icons_texture.loadFromFile(
        "D:/source/repos/Icons" + std::to_string(CELL_SIZE) + ".png"
    );

    icon_sprite.setTexture(icons_texture);

    // Перебираємо кожну клітинку
    for (unsigned char a = 0; a < COLUMNS; a++)
    {
        for (unsigned char b = 0; b < ROWS; b++)
        {
            // Змінюємо положення форми клітини
            cell_shape.setPosition(
                static_cast<float>(CELL_SIZE * a),
                static_cast<float>(CELL_SIZE * b)
            );

            // Якщо поточна клітинка відкрита
            if (1 == get_cell(a, b, cells)->get_is_open())
            {
                // Ми отримуємо кількість мін навколо неї
                unsigned char mines_around =
                    get_cell(a, b, cells)->get_mines_around();

                cell_shape.setFillColor(sf::Color(146, 182, 255));

                // Малюємо клітинку
                i_window.draw(cell_shape);

                // Якщо навколо клітини є хоча б одна міна
                if (0 < mines_around)
                {
                    // Беремо число з текстури іконок і малюємо його
                    icon_sprite.setPosition(
                        static_cast<float>(CELL_SIZE * a),
                        static_cast<float>(CELL_SIZE * b)
                    );

                    icon_sprite.setTextureRect(
                        sf::IntRect(
                            CELL_SIZE * mines_around,
                            0,
                            CELL_SIZE,
                            CELL_SIZE
                        )
                    );

                    i_window.draw(icon_sprite);
                }
            }
            else
            {
                // Якщо клітина закрита
                cell_shape.setFillColor(sf::Color::Black);

                // Ми встановлюємо колір комірки на основі стану миші.
                // Також не змінюємо його колір, якщо гра закінчилася
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

                // Якщо комірка позначена прапорцем
                if (1 == get_cell(a, b, cells)->get_is_flagged())
                {
                    // Беремо зображення прапора і малюємо його
                    icon_sprite.setPosition(
                        static_cast<float>(CELL_SIZE * a),
                        static_cast<float>(CELL_SIZE * b)
                    );

                    icon_sprite.setTextureRect(
                        sf::IntRect(0, 0, CELL_SIZE, CELL_SIZE)
                    );

                    i_window.draw(icon_sprite);
                }
            }

            // Скидаємо стан клітинки миші
            get_cell(a, b, cells)->set_mouse_state(0);

            // Тут малюю ефект, коли обираємо комірку з міною
            if (
                0 != game_over &&
                EFFECT_DURATION > get_cell(a, b, cells)->get_effect_timer()
            )
            {
                // Розраховуємо розмір ефекту
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

                // Таймер ефекту кожної комірки матиме випадкову тривалість
                std::uniform_int_distribution<unsigned short>
                    effect_duration_distribution(1, EFFECT_DURATION - 1);

                // Ми будемо використовувати форму комірки,
                // щоб намалювати ефекти
                cell_shape.setPosition(
                    floor(CELL_SIZE * (0.5f + a) - 0.5f * effect_size),
                    floor(CELL_SIZE * (0.5f + b) - 0.5f * effect_size)
                );

                cell_shape.setSize(
                    sf::Vector2f(effect_size, effect_size)
                );

                // Колір ефекту залежатиме від того,
                // програна чи виграна гра
                if (-1 == game_over)
                {
                    cell_shape.setFillColor(sf::Color::Red);
                }
                else
                {
                    cell_shape.setFillColor(sf::Color::Black);
                }

                // Малюємо ефект
                i_window.draw(cell_shape);

                // Ми скидаємо розмір клітинки
                cell_shape.setSize(
                    sf::Vector2f(CELL_SIZE - 1, CELL_SIZE - 1)
                );

                // Якщо таймер ефекту закінчився
                if (1 == get_cell(a, b, cells)->update_effect_timer())
                {
                    // Ми запускаємо таймер ефекту кожної сусідньої комірки
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
    // Ми не дозволяємо гравцеві позначати клітинки після закінчення гри
    if (0 == game_over)
    {
        get_cell(i_x, i_y, cells)->flag();
    }
}

void Field::open_cell(unsigned char i_x, unsigned char i_y)
{
    // Якщо це перша клітинка, яку ми відкриваємо
    if (0 == first_click)
    {
        // Оголошуємо координатні розподіли
        std::uniform_int_distribution<unsigned short>
            x_distribution(0, COLUMNS - 1);

        std::uniform_int_distribution<unsigned short>
            y_distribution(0, ROWS - 1);

        first_click = 1;

        // Потім генеруємо міни
        for (unsigned short a = 0; a < MINES; a++)
        {
            unsigned char mine_x =
                static_cast<unsigned char>(x_distribution(random_engine));

            unsigned char mine_y =
                static_cast<unsigned char>(y_distribution(random_engine));

            // Якщо у вибраній клітині вже є міна або це клітина,
            // яку гравець хоче відкрити
            if (
                1 == get_cell(mine_x, mine_y, cells)->get_is_mine() ||
                (i_x == mine_x && i_y == mine_y)
            )
            {
                // Ми пробуємо знову
                a--;
            }
            else
            {
                // В іншому випадку встановлюємо міну
                get_cell(mine_x, mine_y, cells)->set_mine();
            }
        }

        // Після того, як ми згенеруємо міни,
        // кожна клітинка підраховує кількість мін навколо неї
        for (Cell& cell : cells)
        {
            cell.count_mines_around(cells);
        }
    }

    // Ми не відкриваємо клітинку, коли гра закінчена
    // або коли клітинку позначено прапорцем
    if (
        0 == game_over &&
        0 == get_cell(i_x, i_y, cells)->get_is_flagged()
    )
    {
        if (1 == get_cell(i_x, i_y, cells)->open(cells))
        {
            // Коли гравець відкриває клітинку з міною,
            // ми встановлюємо гру на -1
            game_over = -1;
        }
        else
        {
            unsigned short total_closed_cells = 0;

            // Підраховуємо, скільки клітинок закрито
            for (Cell& cell : cells)
            {
                total_closed_cells += 1 - cell.get_is_open();
            }

            // Якщо кількість закритих комірок дорівнює загальній кількості мін,
            // ми вважатимемо гру виграною
            if (MINES == total_closed_cells)
            {
                // Ми ставимо гру на 1
                game_over = 1;

                // Потім запускаємо ефект
                get_cell(i_x, i_y, cells)->set_effect_timer(EFFECT_DURATION - 1);
            }
        }
    }
}

void Field::restart()
{
    // Ми перезапускаємо гру лише після її завершення
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