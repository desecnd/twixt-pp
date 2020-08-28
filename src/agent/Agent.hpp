#pragma once

#include "game/Player.hpp"
#include "Hasher.hpp"
#include "Evaluator.hpp"
#include "MoveOrganizer.hpp"

#include <unordered_map>
#include <random>

namespace agent {

    struct Stats {
        int calculatedLeafs;
        int transpositionHits;
        Score bestScore;
        Score worstScore;
        Score positionScore[Board::kRows][Board::kCols];
        int positionOrder[Board::kRows][Board::kCols];

        Stats() : calculatedLeafs(0), transpositionHits(0), bestScore(-kInf), worstScore(kInf) {
            for (int row = 0; row < Board::kRows; row++) 
            for (int col = 0; col < Board::kCols; col++) {
                    positionOrder[row][col] = -1;
                    positionScore[row][col] = -kInf;
            } 
        }
    };

    class Agent : public Player {
    private:
        int perspective_;
        Stats myStats_;
        Hasher hasher_;
        Evaluator evaluator_;
        MoveOrganizer moveOrganizer_;
        std::unordered_map<Hash, Score> transpositionTable_;

        Score evaluateAndRemember(const Board& board);
        void fillMyStats(Board board);
        std::pair<Score, Position> ABNegamax(Board board, int maxDepth, int depth, Score a, Score b, int color);

    public:

        Agent(std::mt19937_64& rng) : perspective_{}, myStats_{}, hasher_{rng}, 
            evaluator_{}, moveOrganizer_{}, transpositionTable_{} {}

        void setPerspective(int player) { perspective_ = (player == 1 ? 1 : -1); }
        int perspective() const { return (perspective_ == 1 ? 1 : 2);  }
        Move getMove(Board board);
        Stats getStats() const { return myStats_; }
    };
}
