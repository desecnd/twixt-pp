#include "Judge.hpp"
#include "BoardEvaluator.hpp"

int Judge::playGame(Player& p1, Player& p2, bool debug = false) {
    Move m1 { "FIRST" };
    Move m2 { "FIRST" };

    int roundsLeft { 12 * 12 - 4 };

    agent::BoardEvaluator be{};

    if ( debug ) { printBoard(std::cout, board_); std::cerr << std::endl; }
    while ( roundsLeft > 0 ) {
        do { m1 = p1.getMove(board_); } while(false);
        applyMove(board_, m1);

        if ( debug ) { printBoard(std::cout, board_); std::cerr << std::endl; }

        if ( board_.isGameOver() ) return 1;

        do { m2 = p2.getMove(board_); } while(false);
        applyMove(board_, m2);

        if ( debug ) { printBoard(std::cout, board_); std::cerr << std::endl; }
        if ( board_.isGameOver() ) return 2;

        roundsLeft -= 2;
    }
    if ( debug ) { printBoard(std::cout, board_); std::cerr << std::endl; }
    return 0;
}