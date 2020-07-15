#include "BoardHasher.hpp"

namespace agent {

    Hash BoardHasher::calculateBoardHash(const Board& board) {
        Hash hash{ 0 };

        for (int r = 0; r < Board::kRows; r++) 
        for (int c = 0; c < Board::kCols; c++) {
            Position current { r, c };
            
            int player = board.pegOwner(current);
            if ( player == 0 ) continue;

            int posIndex  = r * Board::kCols + c; 
            hash ^= hashes_[posIndex][player - 1];

            for (int directionInt = 0; directionInt < DIRECTIONS; directionInt++) {
                Direction direction { static_cast<Direction>(directionInt) };

                if ( board.linkExist(current, direction, player) )
                    hash ^= hashes_[posIndex][(player - 1) * (1 + DIRECTIONS) + (1 + directionInt) ];
            }
        }

        return hash;
    }
}