#pragma once

#include "board/Board.hpp"

namespace agent {
    using Score = float; 

    class Evaluator {
    public:
        Score evaluateBoardScore(const Board& board);
        Score edgeDistanceHeuristic(const Board& board);
        Score exploitationHeuristic(const Board& board);
        Score edgeWinnersHeuristic(const Board& board);
        Score connectedComponentsHeuristic(const Board& board);
        std::vector<std::vector<int>> calculateDistance(const Board& board, int testingPlayer);
        std::vector<std::vector<int>> getEdgeWinners(Board board);
        int simulateExploitation(Board board);
        int countConnectedComponents(const Board& board, int testingPlayer);
        void dfs(const Board& board, std::vector<std::vector<int>>& vis, Position pos, int player);
    };
}