#include "BoardMoveOrganizer.hpp"

namespace agent {
    std::vector<Position> BoardMoveOrganizer::getAvailablePositions(const Board& board) {
        std::vector<Position> positions;
        for (int r = 0; r < Board::kRows; r++) 
        for (int c = 0; c < Board::kCols; c++) {
            Position current { r, c };
            if ( board.pegOwner(current) == 0 )
                positions.push_back(current);
        }
        return positions;
    }
}