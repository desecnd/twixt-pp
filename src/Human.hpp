#pragma once

#include "Player.hpp"
#include "Move.hpp"

class Human : public Player {
private:
    agent::BoardEvaluator be_{};
public:
    void setPerspective(int player) {}
    Move getMove(const Board& board) {
        printBoard(std::cout, board);
        // be_.evaluateBoardScore(board);
        Move myMove;
        // int row, col; std::cin >> row >> col; myMove = posToMove(Position(row, col));
        std::cin >> myMove;
        std::cerr << "Mymove: " << myMove << std::endl;
        return myMove;
    }
};