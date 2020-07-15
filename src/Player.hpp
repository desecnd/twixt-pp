#pragma once

#include "Board.hpp"
#include "Move.hpp"

class Player {
public:
    virtual void setPerspective(int player) = 0;
    virtual Move getMove(const Board& board) = 0;
};