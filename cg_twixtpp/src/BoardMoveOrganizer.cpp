#include "BoardMoveOrganizer.hpp"

namespace agent {
    std::vector<Position> BoardMoveOrganizer::getAvailablePositions(const Board& board) {
        std::vector<Position> positions;

        int cBias { board.currentPlayer() == 1 ? 1 : 0 };
        int rBias { board.currentPlayer() == 2 ? 1 : 0 };

        for (int r = rBias; r < Board::kRows - rBias; r++) 
        for (int c = cBias; c < Board::kCols - cBias; c++) {
            Position current { r, c };
            if ( board.pegOwner(current) == 0 )
                positions.push_back(current);
        }
        return positions;
    }
}