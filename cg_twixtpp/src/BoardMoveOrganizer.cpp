#include "BoardMoveOrganizer.hpp"

namespace agent {
    std::vector<Position> BoardMoveOrganizer::getAvailablePositions(const Board& board) {
        std::vector<Position> empty;
        std::swap(empty, positions);

        for (int r = 0; r < Board::kRows; r++) 
        for (int c = 0; c < Board::kCols; c++) 
            vis[r][c] = 0;

        if ( board.currentPlayer() == 1 ) { 
            for (int i = 0; i < Board::kRows; i++) 
                vis[i][0] = vis[i][Board::kCols - 1] = 1;
        }
        else {
            for (int i = 0; i < Board::kCols; i++) 
                vis[0][i] = vis[Board::kRows - 1][i] = 1;
        }

        int me = board.currentPlayer();
        int opp = board.opponent(me);

        appendCrossMoves(board, opp, 2);
        appendKnightMoves(board, me);
        appendKnightMoves(board, opp);
        appendCrossMoves(board, opp, 1);
        appendCrossMoves(board, opp, 3);
        appendCrossMoves(board, opp, 3);
        appendAll(board, me);

        return positions;
    }

    void BoardMoveOrganizer::appendCrossMoves(const Board& board, int player, int depth) {
        Position up(-1, 0);
        Position left(0, -1);

        for (int r = 0; r < Board::kRows; r++) 
        for (int c = 0; c < Board::kCols; c++) {
            Position pos { r, c }, way, next;

            way = up * depth; next = pos + way;
            if ( board.valid(next) && board.pegOwner(next) == 0 && !vis[next.row()][next.col()] ) {
                vis[next.row()][next.col()] = 1;
                positions.push_back(next);
            }

            way = up * depth; next = pos - way;
            if ( board.valid(next) && board.pegOwner(next) == 0 && !vis[next.row()][next.col()] ) {
                vis[next.row()][next.col()] = 1;
                positions.push_back(next);
            }

            way = left * depth; next = pos + way;
            if ( board.valid(next) && board.pegOwner(next) == 0 && !vis[next.row()][next.col()] ) {
                vis[next.row()][next.col()] = 1;
                positions.push_back(next);
            }

            way = left * depth; next = pos - way;
            if ( board.valid(next) && board.pegOwner(next) == 0 && !vis[next.row()][next.col()] ) {
                vis[next.row()][next.col()] = 1;
                positions.push_back(next);
            }
        }
    }
        


    void BoardMoveOrganizer::appendKnightMoves(const Board& board, int player) {
        for (int r = 0; r < Board::kRows; r++) 
        for (int c = 0; c < Board::kCols; c++) {
            Position pos { r, c };

            if ( board.pegOwner(pos) != player ) continue;

            for (int directionInt = 0; directionInt < DIRECTIONS; directionInt++) {
                Direction direction { static_cast<Direction>(directionInt) };

                Position next { pos + getVector(direction) };

                if ( board.valid(next) && board.pegOwner(next) == 0 && !vis[next.row()][next.col()] ) {
                    vis[next.row()][next.col()] = 1;
                    positions.push_back(next);
                }
            }
        }
    }

    void BoardMoveOrganizer::appendAll(const Board& board, int player) {
        int cBias { player == 1 ? 1 : 0 };
        int rBias { player == 2 ? 1 : 0 };

        for (int r = rBias; r < Board::kRows - rBias; r++) 
        for (int c = cBias; c < Board::kCols - cBias; c++) {
            Position current { r, c };
            if ( board.pegOwner(current) == 0 && !vis[r][c]) {
                positions.push_back(current);
                vis[r][c] = 1;
            }
        }
    }
}