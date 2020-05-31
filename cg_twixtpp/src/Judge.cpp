#include "Judge.hpp"

int Judge::playGame(Player& p1, Player& p2, bool debug = false) {
    Move m1 { "FIRST" };
    Move m2 { "FIRST" };

    int roundsLeft { 12 * 12 - 4 };
    if ( debug ) printCurrentBoard();
    while ( roundsLeft > 0 ) {
        m1 = p1.getMove(m2);
        applyMove(board_, m1);
        if ( debug ) printCurrentBoard();
        if ( board_.isGameOver() ) return 1;

        m2 = p2.getMove(m1);
        applyMove(board_, m2);
        if ( debug ) printCurrentBoard();
        if ( board_.isGameOver() ) return 2;

        roundsLeft -= 2;
    }
    if ( debug ) printCurrentBoard();
    return 0;
}

void Judge::printCurrentBoard() {
    for (int r = 0; r < Board::kRows; r++ ) {
        for (int c = 0; c < Board::kCols; c++ ) {
            std::cout << board_.pegOwner(Position(r, c)) << " ";
        }
        std::cout << std::endl;
    }

    std::vector<std::vector<int>> links;
    for (int r = 0; r < Board::kRows; r++ ) {
    for (int c = 0; c < Board::kCols; c++ ) {
        Position current { r, c }; 
        int player = board_.pegOwner(current);
        if ( player == 0 )
            continue;
        
        for (int directionInt = 0; directionInt < DIRECTIONS/2; directionInt++) {
            Direction direction { static_cast<Direction>(directionInt) };
            if ( board_.linkExist(current, direction, player) ) {
                Position other { current + getVector(direction) };
                links.push_back({ player, current.row(), current.col(), other.row(), other.col() });
            }
        }
    }
    }
    std::cout << links.size() << std::endl;
    for ( auto& v : links) {
        for (int i : v ) {
            std::cout << i << " ";
        }
        std::cout << std::endl;

    }
    std::cerr << std::endl;
}