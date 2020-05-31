#pragma once

#include "Direction.hpp"
#include "Position.hpp"
#include "Collisions.hpp"

#include <iostream>
#include <queue>
// player 1 or 2 
// position taken on empty


class Board {
public:
    static constexpr int kRows { 12 };
    static constexpr int kCols { 12 };
    static constexpr CollisionTable collisionTable_{};
private:
    int currentPlayer_ { 1 };
    std::array<std::uint8_t, kRows * kCols> pegs_ {};
    std::array<std::uint16_t, kRows * kCols> links_ {};


    int index(Position pos) const { 
        return pos.row() * kCols + pos.col(); 
    }
    int opponent(int player) const { 
        return 3 - player; 
    }

    bool valid(Position pos) const { 
        return (pos.row() >= 0 && pos.row() < kRows && pos.col() >= 0 && pos.col() < kCols );
    }

    void markPeg(Position pos, int player) { 
        pegs_[index(pos)] |= (1 << (player - 1)); 
    }

    void markLink(Position pos, Direction dir, int player) { 
        links_[index(pos)] |= (1 << (DIRECTIONS * (player - 1) + static_cast<int>(dir)));
        links_[index(pos + getVector(dir))] |= (1 << (DIRECTIONS * (player - 1) + static_cast<int>(opposite(dir))));

        // std::cerr << "{Created link: " << pos << " -> " << static_cast<int>(dir) << ", for player" << player << "}";
        // std::cerr << " == {Created link: " << (pos + getVector(dir)) << " -> " << static_cast<int>(opposite(dir)) << ", for player" << player << "}";
    }

    bool linkPossible(Position pos, Direction direction, int player) const {
        for ( const Link& link : collisionTable_.collisions(direction) ) {
            if ( valid(pos + link.position) && linkExist(pos + link.position, link.direction, opponent(player)) ) 
                return false;
        }
        return true;
    }

    void dfs(std::vector<std::vector<int>>& vis, Position pos, int player);
public:
    Board() = default;

    bool linkExist(Position pos, Direction dir, int player) const {
        return (links_[index(pos)] & ( 1<<(DIRECTIONS * (player - 1) + static_cast<int>(dir)) ));
    }

    int pegOwner(Position pos) const {
        if ( pegs_[index(pos)] & 2 ) return 2;
        else if ( pegs_[index(pos)] & 1 ) return 1;
        else return 0;
    }

    void takePeg(Position pos);
    bool isGameOver();
    void debug();
};


extern constexpr CollisionTable Board::collisionTable_;