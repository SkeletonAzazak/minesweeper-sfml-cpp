#include <vector>

#include "Cell.h"
#include "GetCell.h"
#include "Global.h"

Cell* get_cell(
    unsigned char i_x,
    unsigned char i_y,
    std::vector<Cell>& i_cells
)
{
    return &i_cells[i_x + COLUMNS * i_y];
}