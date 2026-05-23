#include <vector>
#include "Cell.h"
#include "GetCell.h"
#include "Global.h"

Cell::Cell(unsigned char i_x, unsigned char i_y) :
    mines_around(0),
    x(i_x),
    y(i_y)
{
    // У мене вже є функція скидання,
    // то чому б не використати її під час створення об’єкта?
    reset();
}

bool Cell::get_is_flagged()
{
    // Повертає, чи позначено клітинку чи ні
    return is_flagged;
}

bool Cell::get_is_mine()
{
    // Повертає, чи є в клітині міна чи ні
    return is_mine;
}

bool Cell::get_is_open()
{
    // Повертає, чи є клітина відкритою чи ні
    return is_open;
}

bool Cell::open(std::vector<Cell>& i_cells)
{
    // Не можна відкрити клітинку, яка вже відкрита
    if (0 == is_open)
    {
        is_open = 1;

        // Якщо в цій клітинці та клітинах навколо неї немає мін,
        // ми їх відкриємо
        if (0 == is_mine && 0 == mines_around)
        {
            for (char a = -1; a < 2; a++)
            {
                for (char b = -1; b < 2; b++)
                {
                    // Переконавшись, що ми не виходимо за межі ігрового поля
                    if (
                        (0 == a && 0 == b) ||
                        (0 > a + x || 0 > b + y ||
                        COLUMNS == a + x || ROWS == b + y)
                    )
                    {
                        continue;
                    }

                    // Відкриваємо сусідні комірки
                    get_cell(a + x, b + y, i_cells)->open(i_cells);
                }
            }
        }

        // Починаємо ефект, якщо в клітині є міна
        effect_timer -= is_mine;

        return is_mine;
    }

    return 0;
}

bool Cell::update_effect_timer()
{
    // Якщо таймер ефекту не закінчився
    if (0 < effect_timer)
    {
        // Тоді зменшуємо його
        effect_timer--;

        if (0 == effect_timer)
        {
            // Ми повернемо лише 1 раз,
            // відразу після закінчення таймера
            return 1;
        }
    }

    return 0;
}

unsigned char Cell::get_effect_timer()
{
    return effect_timer;
}

unsigned char Cell::get_mines_around()
{
    return mines_around;
}

unsigned char Cell::get_mouse_state()
{
    return mouse_state;
}

void Cell::count_mines_around(std::vector<Cell>& i_cells)
{
    // Починаємо рахувати з 0
    mines_around = 0;

    // Якщо в цій комірці є міна,
    // то немає сенсу рахувати міни навколо неї
    if (0 == is_mine)
    {
        for (char a = -1; a < 2; a++)
        {
            for (char b = -1; b < 2; b++)
            {
                if (
                    (0 == a && 0 == b) ||
                    (0 > a + x || 0 > b + y ||
                    COLUMNS == a + x || ROWS == b + y)
                )
                {
                    continue;
                }

                // Якщо у сусіда є міна
                if (1 == get_cell(a + x, b + y, i_cells)->get_is_mine())
                {
                    // Збільшуємо цю змінну
                    mines_around++;
                }
            }
        }
    }
}

void Cell::flag()
{
    // Не можна позначати клітинку, яка вже відкрита
    if (0 == is_open)
    {
        is_flagged = 1 - is_flagged;
    }
}

void Cell::reset()
{
    // Встановлюємо для кожної змінної значення 0
    is_flagged = 0;
    is_mine = 0;
    is_open = 0;
    mines_around = 0;

    // Крім таймера ефекту
    effect_timer = EFFECT_DURATION;

    mouse_state = 0;
}

void Cell::set_effect_timer(unsigned char i_effect_timer)
{
    effect_timer = i_effect_timer;
}

void Cell::set_mine()
{
    is_mine = 1;
}

void Cell::set_mouse_state(unsigned char i_mouse_state)
{
    mouse_state = i_mouse_state;
}