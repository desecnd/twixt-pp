#include "Agent.hpp"

#include <utility>

namespace agent {

    Score Agent::evaluateAndRemember(const Board& board) {
        Hash boardHash { boardHasher_.calculateBoardHash(board) };

        if ( transpositionTable_.find(boardHash) == transpositionTable_.end() ) {
            Score boardScore { boardEvaluator_.evaluateBoardScore(board) };
            transpositionTable_[boardHash] = boardScore; 
        }
        return transpositionTable_[boardHash];
    }

    std::pair<Score, Position> Agent::ABNegamax(Board board, int maxDepth, int depth, Score alpha, Score beta) {
        if ( depth == maxDepth || board.isGameOver() ) 
            return std::make_pair( evaluateAndRemember(board), Position(-1,-1)); 

        Score bestScore = -1e9;
        Position bestPosition { -1, -1 };

        std::vector<Position> positions { boardMoveOrganizer_.getAvailablePositions(board) };
        for ( Position currentPosition : positions ) {
            Board newBoard(board);
            newBoard.takePeg(currentPosition);

            Position tempPos; 
            Score recursedScore;

            std::tie(recursedScore, tempPos) = ABNegamax(newBoard, 
                    maxDepth, depth + 1, -beta, -std::max(alpha, bestScore));

            Score currentScore = -recursedScore;

            if ( currentScore > bestScore ) {
                bestScore = currentScore;
                bestPosition = currentPosition;
            }

            if ( bestScore >= beta )
                break;
        }

        return std::make_pair(bestScore, bestPosition);
    }

    Move Agent::getMove(Move lastOppMove) {
        applyMove(board_, lastOppMove);

        Score bestScore;
        Position bestPosition;
        std::tie(bestScore, bestPosition) = ABNegamax(board_, 2, 0, -1e9, 1e9);
        return posToMove(bestPosition);
    }
}