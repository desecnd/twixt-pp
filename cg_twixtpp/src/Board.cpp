#include "Board.hpp"

void Board::makeMove(Position pos) {
    markPeg(pos, currentPlayer_); 

    for (int directionInt = 0; directionInt < DIRECTIONS; directionInt++) {
        Direction direction { static_cast<Direction>(directionInt) };
        Position away { pos + getVector(direction) };

        if ( valid(away) && pegOwner(away) == 0 ) {
            if ( linkPossible(pos, direction, currentPlayer_) ) 
                markLink(pos, direction, currentPlayer_);
        }
    }

    currentPlayer_ = opponent(currentPlayer_);
}


void Board::debug() {
    for (int r = 0; r < rows_; r++) {
        for (int c = 0; c < cols_; c++) {
            std::cerr << pegOwner(Position(r, c)) << " ";
            // std::cerr << index(Position(r, c)) << " ";
        }
        std::cerr << "\n";
    }
} 
constexpr CollisionTable Board::collisionTable_;