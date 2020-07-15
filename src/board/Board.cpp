#include "Board.hpp"

constexpr CollisionTable Board::collisionTable_;

void Board::takePeg(Position pos, bool noSwap ) {
    markPeg(pos, currentPlayer_); 

    for (int directionInt = 0; directionInt < DIRECTIONS; directionInt++) {
        Direction direction { static_cast<Direction>(directionInt) };
        Position away { pos + getVector(direction) };

        if ( possible(away, currentPlayer_) && pegOwner(away) == currentPlayer_ ) {
            if ( linkPossible(pos, direction, currentPlayer_) ) 
                markLink(pos, direction, currentPlayer_);
        }
    }

    if ( !noSwap)
        currentPlayer_ = opponent(currentPlayer_);
}

void Board::dfs(std::vector<std::vector<int>>& vis, Position pos, int player) const {
    vis[pos.row()][pos.col()] = 1;

    for (int directionInt = 0; directionInt < DIRECTIONS; directionInt++) {
        Direction direction { static_cast<Direction>(directionInt) };

        Position next { pos + getVector(direction) };
        if ( possible(next, player) && !vis[next.row()][next.col()] && pegOwner(next) == player && linkExist(pos, direction, player))
            dfs(vis, next, player);
    }
}

int Board::isGameOver() const {
    std::vector<std::vector<int>> vis(kRows, std::vector<int>(kCols, 0));

    for (int col = 1; col < kCols - 1; col++)
        if ( pegOwner(Position(0, col)) == 1 ) 
            dfs(vis, Position(0, col), 1);

    for (int col = 1; col < kCols - 1; col++)
        if  ( vis[kRows - 1][col] ) return 1;

    for (int row = 1; row < kRows - 1; row++)
        if ( pegOwner(Position(row, 0)) == 2 ) 
            dfs(vis, Position(row, 0), 2);

    for (int row = 1; row < kRows - 1; row++)
        if  ( vis[row][kCols - 1] ) return 2;

    return 0;
}

void Board::debug() {
    for (int r = 0; r < kRows; r++) {
        for (int c = 0; c < kCols; c++) {
            std::cerr << pegOwner(Position(r, c)) << " ";
            // std::cerr << index(Position(r, c)) << " ";
        }
        std::cerr << "\n";
    }
} 

void printBoard(std::ostream& out, const Board& board) {
    for (int r = 0; r < Board::kRows; r++ ) {
        for (int c = 0; c < Board::kCols; c++ ) {
            out << board.pegOwner(Position(r, c)) << " ";
        }
        out << std::endl;
    }

    std::vector<std::vector<int>> links;
    for (int r = 0; r < Board::kRows; r++ ) {
    for (int c = 0; c < Board::kCols; c++ ) {
        Position current { r, c }; 
        int player = board.pegOwner(current);
        if ( player == 0 )
            continue;
        
        for (int directionInt = 0; directionInt < DIRECTIONS/2; directionInt++) {
            Direction direction { static_cast<Direction>(directionInt) };
            if ( board.linkExist(current, direction, player) ) {
                Position other { current + getVector(direction) };
                links.push_back({ player, current.row(), current.col(), other.row(), other.col() });
            }
        }
    }
    }
    out << links.size() << std::endl;
    for ( auto& v : links) {
        for (int i : v ) {
            out << i << " ";
        }
        out << std::endl;
    }
}