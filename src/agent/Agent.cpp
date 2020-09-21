#include "Agent.hpp"

#include <utility>

namespace agent {
    Score Agent::evaluateAndRemember(const Board& board) {
        Hash boardHash { hasher_.calculateBoardHash(board) };

        if ( transpositionTable_.find(boardHash) == transpositionTable_.end() ) {
            Score boardScore { evaluator_.evaluateBoardScore(board) };
            transpositionTable_[boardHash] = boardScore; 
            myStats_.transpositionMisses++;
        }

        return transpositionTable_[boardHash];
    }

    void Agent::fillMyStats(Board board) {
        myStats_.raport = evaluator_.getRaport(board);

        std::vector<Position> positions { moveOrganizer_.getAvailablePositions(board) };

        for (int i = 0; i < positions.size(); i++) {
            Board newBoard(board);

            Position currentPosition = positions[i];
            newBoard.takePeg(currentPosition);

            Score moveScore = perspective_ * evaluateAndRemember(newBoard);
            myStats_.positionOrder[currentPosition.row()][currentPosition.col()] = i;
            myStats_.positionScore[currentPosition.row()][currentPosition.col()] = moveScore;

            if ( moveScore > -kSureWin && moveScore < kSureWin ) {
                myStats_.bestScore = std::max(myStats_.bestScore, moveScore);
                myStats_.worstScore = std::min(myStats_.worstScore, moveScore);
            }
        }
    }

    std::pair<Score, Position> Agent::ABNegamax(Board board, int maxDepth, int depth, Score alpha, Score beta, int color) {
        myStats_.visitedNodes++;

        if ( depth == maxDepth || board.isGameOver() ) {
            myStats_.calculatedLeafs++;
            return {color * evaluateAndRemember(board), {-1, -1}}; 
        }

        Score bestScore = -kInf;
        Position bestPosition { -1, -1 };

        std::vector<Position> positions { moveOrganizer_.getAvailablePositions(board) };

        for ( int i = 0; i < positions.size(); i++ ) {
            Board newBoard(board);
            newBoard.takePeg(positions[i]);



            auto[recursedScore, tempPos] = ABNegamax(newBoard, 
                    maxDepth, depth + 1, -beta, -std::max(alpha, bestScore), -color);

            Score currentScore = -recursedScore;

            if ( depth == 0 ) {
                myStats_.positionOrder[positions[i].row()][positions[i].col()] = i;
                myStats_.positionScore[positions[i].row()][positions[i].col()] = currentScore;
                if ( currentScore > -kSureWin && currentScore < kSureWin ) {
                    myStats_.bestScore = std::max(myStats_.bestScore, currentScore);
                    myStats_.worstScore = std::min(myStats_.worstScore, currentScore);
                }
            }

            if ( currentScore > bestScore ) {
                bestScore = currentScore;
                bestPosition = positions[i];
            }

            if ( bestScore >= beta ) 
                break;
        }

        return {bestScore, bestPosition};
    }

    Move Agent::getMove(Board board) {
        static int round = 0;

        round += 2;

        Stats newStats;
        myStats_ = newStats;
        int n = Board::kRows * (Board::kCols - 2) - round;
        myStats_.treeNodes = n * n + n + 1;
        myStats_.raport = evaluator_.getRaport(board);
        auto[bestScore, bestPosition] = ABNegamax(board, 2, 0, -kInf, kInf, perspective_);
        return posToMove(bestPosition);
    }

}
