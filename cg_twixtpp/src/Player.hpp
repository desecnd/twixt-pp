#pragma once

#include "Move.hpp"

class Player {
public:
    virtual Move getMove(Move lastOpponentMove) = 0;
};