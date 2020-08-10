#include "Agent.hpp"

#include <utility>

namespace agent {
    Score Agent::evaluateAndRemember(const Board& board) {
        Hash boardHash { hasher_.calculateBoardHash(board) };

        if ( transpositionTable_.find(boardHash) == transpositionTable_.end() ) {
            Score boardScore { evaluator_.evaluateBoardScore(board) };
            transpositionTable_[boardHash] = boardScore; 
        }
        else {
            myStats_.transpositionHits++;
        }

        return transpositionTable_[boardHash];
    }

    void Agent::fillMyStats(Board board) {
        std::vector<Position> positions { moveOrganizer_.getAvailablePositions(board) };

        for (int i = 0; i < positions.size(); i++) {
            Board newBoard(board);

            Position currentPosition = positions[i];
            newBoard.takePeg(currentPosition);

            Score moveScore = evaluateAndRemember(newBoard);
            myStats_.positionOrder[currentPosition.row()][currentPosition.col()] = i;
            myStats_.positionScore[currentPosition.row()][currentPosition.col()] = moveScore;

            myStats_.bestScore = std::max(myStats_.bestScore, moveScore);
            myStats_.worstScore = std::min(myStats_.worstScore, moveScore);
        }
    }

    std::pair<Score, Position> Agent::ABNegamax(Board board, int maxDepth, int depth, Score alpha, Score beta, int color) {
        if ( depth == maxDepth || board.isGameOver() ) {
            myStats_.calculatedLeafs++;
            return {color * evaluateAndRemember(board), {-1, -1}}; 
        }

        Score bestScore = -1e9;
        Position bestPosition { -1, -1 };

        std::vector<Position> positions { moveOrganizer_.getAvailablePositions(board) };

        for ( Position currentPosition : positions ) {
            Board newBoard(board);
            newBoard.takePeg(currentPosition);

            auto[recursedScore, tempPos] = ABNegamax(newBoard, 
                    maxDepth, depth + 1, -beta, -std::max(alpha, bestScore), -color);

            Score currentScore = -recursedScore;

            if ( currentScore > bestScore ) {
                bestScore = currentScore;
                bestPosition = currentPosition;
            }

            if ( bestScore >= beta ) 
                break;
        }

        return {bestScore, bestPosition};
    }

    Move Agent::getMove(Board board) {
        Stats newStats;
        myStats_ = newStats;
        auto[bestScore, bestPosition] = ABNegamax(board, 2, 0, -1e9, 1e9, perspective_);
        fillMyStats(board);
        return posToMove(bestPosition);
    }

}
