#pragma once

#include "Player.hpp"
#include "Move.hpp"

class Human : public Player {
public:
    Move getMove(Move lastOppMove) {
        Move myMove;
        std::cin >> myMove;
        return myMove;
    }
};