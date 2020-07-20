#pragma once

#include "board/Board.hpp"
#include "Move.hpp"

class Player {
public:
    virtual void setPerspective(int player) = 0;
    virtual Move getMove(Board board) = 0;
};