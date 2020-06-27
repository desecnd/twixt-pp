#include "Position.hpp"
#include "Direction.hpp"
#include "Board.hpp"
#include "Judge.hpp"
#include "Agent.hpp"
#include "Human.hpp"
#include "BoardHasher.hpp"

#include <iostream>
#include <random>

template <class T>
constexpr void test_helper(T&&) {}

#define IS_CONSTEXPR(...) noexcept(test_helper(__VA_ARGS__))

int main() {
    using namespace agent;

    std::mt19937_64 rng{ 123 };

    Judge judge{};
    Agent myAgent{ rng };
    Human me{ };

    Position p(2, 5);
    // std::cout << p << " " << posToMove(p) << " " << moveToPos(posToMove(p)) << "n";

    Board board;

    /*
    for (int directionInt = 0; directionInt < DIRECTIONS; directionInt++) {
        Direction direction { static_cast<Direction>(directionInt) };
        std::cout << "Direction: " << directionInt << "\n";
        for (const Link& link : board.collisionTable_.collisions(direction)) {
            std::cout << link.position << " " << static_cast<int>(link.direction) << "\n";
        }
    }
    */

    return judge.playGame(me, myAgent, true);

    // std::cout << boardHasher.calculateBoardHash(board) << "\n";
    /*
    for (const Link& link : CT.collisions(Direction::right) ) {
        std::cout << link.position << "\n";
    }
    */
}