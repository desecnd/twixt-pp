#pragma once

#include "board/Board.hpp"

namespace agent {
    using Score = int; 

    const Score kInf = 1e9;
    const Score kWin = 1e8;
    const Score kSureWin = 1e7;

    class Evaluator {
    public:
        Score evaluateBoardScore(const Board& board);
        Score edgeDistanceHeuristic(const Board& board);
        Score exploitationHeuristic(const Board& board);
        Score edgeWinnersHeuristic(const Board& board);
        Score pieceHeuristic(const Board& board);
        Score connectedComponentsHeuristic(const Board& board);
        Score minimumLinksHeuristic(const Board& board);
        std::vector<std::vector<int>> calculateDistance(const Board& board, int testingPlayer);
        std::vector<std::vector<int>> getEdgeWinners(Board board);
        int dijkstraMinimumLinks(const Board& board, int player);
        int simulateExploitation(Board board);
        int countConnectedComponents(const Board& board, int testingPlayer);
        void dfs(const Board& board, std::vector<std::vector<int>>& vis, Position pos, int player);
        std::string getRaport(const Board& board);
    };
}