#pragma once

#include "Player.hpp"
#include "BoardHasher.hpp"
#include "BoardEvaluator.hpp"
#include "BoardMoveOrganizer.hpp"

#include <unordered_map>
#include <random>

namespace agent {
    class Agent : public Player {
    private:
        Board board_;
        BoardHasher boardHasher_;
        BoardEvaluator boardEvaluator_;
        BoardMoveOrganizer boardMoveOrganizer_;
        std::unordered_map<Hash, Score> transpositionTable_;

        Score evaluateAndRemember(const Board& board);
        std::pair<Score, Position> ABNegamax(Board board, int maxDepth, int depth, Score a, Score b);
    public:
        Agent(std::mt19937_64& rng) : board_{}, boardHasher_{rng}, 
            boardEvaluator_{}, boardMoveOrganizer_{}, transpositionTable_{} {}
        Move getMove(Move lastOppMove);
};

}

/*
std::pair<Position,float> ABNegamax(Board board, int maxDepth, int depth, float alpha, float beta) {
    if ( depth == maxDepth || board.isGameOver() ) 
        return board.evaluate(); 

    float bestScore = -INF;
    auto moves { board.getMoves() };
    for ( auto move : moves ) {
        Board newBoard(board);
        newBoard.move(move);

        Position tempMove{};
        float recursedScore { ABNegamax(newBoard, maxDepth, depth + 1, -beta, -std::max(alpha, bestScore), tempMove) };
        float currentScore = -recursedScore;

        if ( currentScore > bestScore ) {
            bestScore = currentScore;
            bestMove = move;
        }

        if ( bestScore >= beta )
            break;
    }

    return bestScore
}
*/
