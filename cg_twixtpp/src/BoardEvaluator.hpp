#pragma once

#include "Board.hpp"

namespace agent {
    using Score = float; 

    class BoardEvaluator {
    public:
        Score evaluateBoardScore(const Board& board);
    };
}