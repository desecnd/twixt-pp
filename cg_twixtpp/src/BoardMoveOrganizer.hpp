#pragma once

#include "Board.hpp"

#include <vector>

namespace agent {
    class BoardMoveOrganizer {
        std::vector<std::vector<int>> vis;
        std::vector<Position> positions;

        void appendKnightMoves(const Board& board, int player);
        void appendCrossMoves(const Board& board, int player, int depth);
        void appendAll(const Board& board, int player);

    public:
        BoardMoveOrganizer() : vis(Board::kRows, std::vector<int>(Board::kCols, 0)) {}
        std::vector<Position> getAvailablePositions(const Board& board);
    };
}