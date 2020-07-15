#include "Position.hpp"

#include <iostream>

std::ostream &operator<<(std::ostream& out, const Position& pos) {
    out << "(" << pos.row() << " " << pos.col() << ")";
    return out;
}