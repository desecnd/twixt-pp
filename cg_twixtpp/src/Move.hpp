#pragma once

#include "Position.hpp"
#include "Board.hpp"

#include <string>

using Move = std::string;

bool applyMove(Board& board, Move move);
Move posToMove(Position pos);
Position moveToPos(Move move);