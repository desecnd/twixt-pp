#pragma once

#include "Board.hpp"

#include <vector>

namespace agent {
    class BoardMoveOrganizer {
    public:
        BoardMoveOrganizer() = default;
        std::vector<Position> getAvailablePositions(const Board& board);
    };
}