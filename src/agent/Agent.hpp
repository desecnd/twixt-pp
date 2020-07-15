#pragma once

#include "game/Player.hpp"
#include "Hasher.hpp"
#include "Evaluator.hpp"
#include "MoveOrganizer.hpp"

#include <unordered_map>
#include <random>

namespace agent {
    class Agent : public Player {
    private:
        int perspective_;
        Board board_;
        Hasher hasher_;
        Evaluator evaluator_;
        MoveOrganizer moveOrganizer_;
        std::unordered_map<Hash, Score> transpositionTable_;

        Score evaluateAndRemember(const Board& board);
        std::pair<Score, Position> ABNegamax(Board board, int maxDepth, int depth, Score a, Score b, int color);
    public:
        Agent(std::mt19937_64& rng) : perspective_{}, board_{}, hasher_{rng}, 
            evaluator_{}, moveOrganizer_{}, transpositionTable_{} {}

        void setPerspective(int player) { perspective_ = (player == 1 ? 1 : -1); }
        Move getMove(const Board& board);
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
