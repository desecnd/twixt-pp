#include "Move.hpp"

#include <cassert>

bool applyMove(Board& board, Move move) {
    if ( move == "FIRST" ) {
        return true;
    }
    else if ( move == "SWAP" ) {
        assert("The hell should i do with SWAP?");
        return true;
    }
    else {
        Position pos { moveToPos(move) };
        if ( board.possible(pos, board.currentPlayer())) {
            board.takePeg(pos);
            return true;
        }
        else {
            return false;
        }
    }
}

Move posToMove(Position pos) {
    Move move(1, 'A' + pos.col()); 
    move += std::to_string(pos.row() + 1);
    return move;
}

Position moveToPos(Move move) {
    Position pos { std::stoi(move.substr(1, move.size() - 1)) - 1, move[0] - 'A' };
    return pos;
}

