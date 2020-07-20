#pragma once

#include "Player.hpp"
#include "Move.hpp"

class Human : public Player {
public:
    void setPerspective(int player) {}
    Move getMove(Board board) {
        printBoard(std::cout, board);
        // be_.evaluateBoardScore(board);
        Move myMove;
        // int row, col; std::cin >> row >> col; myMove = posToMove(Position(row, col));
        std::cin >> myMove;
        std::cerr << "Mymove: " << myMove << std::endl;
        return myMove;
    }
};