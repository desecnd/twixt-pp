#pragma once

#include "Board.hpp"
#include "Bot.hpp"
#include "Move.hpp"
#include "Player.hpp"

class Judge {
private:
    Board board_;
public:
    int playGame(Player& p1, Player& p2, bool debug);
};