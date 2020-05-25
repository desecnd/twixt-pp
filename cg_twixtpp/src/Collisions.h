#pragma once

#include "Direction.hpp"
#include "Position.hpp"

struct Link {
    Position position{};
    Direction direction{};

    constexpr Link() noexcept = default;
    constexpr Link(Position pos, Direction dir) : position{pos}, direction{dir} {}
};

constexpr inline Link rotate(const Link& link) {
    return Link(rotate(link.position), rotate(link.direction));
}

class CollisionTable {
public:
    using CollidingLinks = std::array<Link, 9>;

private:
    // CollidingLinks collisions_[9];
    // Woriking only for C++17
    std::array<CollidingLinks, 9> collisions_;

public:
    constexpr CollisionTable() : collisions_{} {
        collisions_[0] = {{ 
            { {-1, 1}, Direction::sright },
            { {-1, 1}, Direction::down },
            { {-1, 1}, Direction::sdown },
            { {-1, 0}, Direction::sright },
            { {-1, 0}, Direction::down },
            { { 0, 1}, Direction::up },
            { { 0, 1}, Direction::sup },
            { { 0, 1}, Direction::sleft },
            { { 0, 2}, Direction::up } 
        }};

        collisions_[1] = {{ 
            { { 1, 1}, Direction::right },
            { { 1, 1}, Direction::sup },
            { { 1, 1}, Direction::up },
            { { 1, 0}, Direction::right },
            { { 1, 0}, Direction::sup },
            { { 0, 1}, Direction::sdown },
            { { 0, 1}, Direction::down },
            { { 0, 1}, Direction::left },
            { { 0, 2}, Direction::sdown } 
        }};

        Direction curr1 { Direction::right };
        Direction curr2 { Direction::sright };
        Direction next1 { rotate(curr1) };
        Direction next2 { rotate(curr2) };

        while ( next1 != Direction::right && next2 != Direction::sright ) {
            collisions_[static_cast<int>(next1)] = collisions_[static_cast<int>(curr1)];

            for(Link& link : collisions_[static_cast<int>(next1)]) 
                link = rotate(link);


            collisions_[static_cast<int>(next2)] = collisions_[static_cast<int>(curr2)];
            for(Link& link : collisions_[static_cast<int>(next2)]) 
                link = rotate(link);

            curr1 = next1;
            curr2 = next2;
            next1 = rotate(next1);
            next2 = rotate(next2);
        }
    }

    const CollidingLinks& collisions(Direction direction) const { return collisions_[static_cast<int>(direction)]; }
};