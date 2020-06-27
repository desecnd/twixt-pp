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

    return judge.playGame(me, myAgent, true);
}