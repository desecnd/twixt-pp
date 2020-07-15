#pragma once

#include "board/Board.hpp"

#include <random>

namespace agent {
    using Hash = std::uint64_t;

    class Hasher {
    public:
        static constexpr int kPositions { Board::kRows *  Board::kCols }; 
        static constexpr int kPawns { 2 * ( 1 + DIRECTIONS ) };
    private:
        Hash hashes_[kPositions][kPawns];
    public:
        Hasher(std::mt19937_64& rng) : hashes_{} {
            std::uniform_int_distribution<Hash> distr{0u, std::numeric_limits<Hash>::max()};
            for (int i = 0; i < kPositions; i++)
            for (int j = 0; j < kPawns; j++)
                hashes_[i][j] = distr(rng);
        }

        Hash calculateBoardHash(const Board& board);
    };
}