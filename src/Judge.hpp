#pragma once

#include "Board.hpp"
#include "Move.hpp"
#include "Player.hpp"

class Judge {
private:
    Board board_;
    void printCurrentBoard();

public:
    int playGame(Player& p1, Player& p2, bool debug);
};