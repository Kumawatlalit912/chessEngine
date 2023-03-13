/*
    File: gui.h

    Interactive graphical interface of a chess board.

    This is temporarily a quick and dirty solution.

    Author: Ken Rouwas
    Date 2017-09-15
*/

#pragma once

#include <vector>
#include "board.h"
#include "move.h"

namespace Chess {

    void print_board(const Board&, Move last_move);

    /* Returns when one of the provided valid moves is read */
    Move read_move(const MoveSet& valid_moves, Color turn);

}
