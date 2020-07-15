#pragma once

#include "Position.hpp"

#include <array>

constexpr int DIRECTIONS { 8 };

enum class Direction {
    right,
    sright,
    up,
    sup,
    left,
    sleft,
    down,
    sdown
};

constexpr std::array<Position, DIRECTIONS> directionVectors
{{ {-1, 2}, { 1, 2}, {-2,-1}, {-2, 1}, { 1,-2}, {-1,-2}, {2, 1}, {2,-1} }};

constexpr inline Position getVector(Direction direction) { return directionVectors[static_cast<int>(direction)]; }

constexpr inline Direction& operator++(Direction& direction) {
    return direction = (direction == Direction::sdown ? 
        Direction::right : static_cast<Direction>(static_cast<int>(direction) + 1));
}

constexpr inline Direction rotate(Direction direction) {
    Direction next { direction };
    ++(++next);
    return next;
}

constexpr inline Direction opposite(Direction direction) {
    return rotate(rotate(direction));
}