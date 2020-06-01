#pragma once

#include "Board.hpp"
#include "Move.hpp"

class Player {
public:
    virtual Move getMove(const Board& board) = 0;
};