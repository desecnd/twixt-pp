#include "Move.hpp"

void applyMove(Board& board, Move move) {
    if ( move == "FIRST" ) {
        return;
    }
    else if ( move == "SWAP" ) {
        assert("The hell should i do with SWAP?");
        return;
    }
    else {
        Position pos { moveToPos(move) };
        board.takePeg(pos);
    }
}

Move posToMove(Position pos) {
    Move move(1, 'A' + pos.col()); 
    move += std::to_string(pos.row() + 1);
    return move;
}

Position moveToPos(Move move) {
    Position pos { move[0] - 'A', std::stoi(move.substr(1, move.size() - 1)) - 1 };
    return pos;
}

