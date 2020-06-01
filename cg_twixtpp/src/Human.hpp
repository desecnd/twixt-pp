#pragma once

#include "Player.hpp"
#include "Move.hpp"

class Human : public Player {
public:
    Move getMove(const Board& board) {
        Move myMove;
        std::cin >> myMove;
        return myMove;
    }
};