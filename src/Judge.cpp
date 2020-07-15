#include "Judge.hpp"
#include "BoardEvaluator.hpp"

int Judge::playGame(Player& p1, Player& p2, bool debug = false) {
    Move m1 { "FIRST" };
    Move m2 { "FIRST" };

    p1.setPerspective(1);
    p2.setPerspective(2);

    std::cerr << "Kappa" << std::endl;

    int roundsLeft { 12 * 12 - 4 };
    while ( roundsLeft > 0 ) {
        do { 
            m1 = p1.getMove(board_); 
        } while( !applyMove(board_, m1) );

        if ( board_.isGameOver() ) return 1;

        do { 
            m2 = p2.getMove(board_); 
        } while( !applyMove(board_, m2) );

        if ( board_.isGameOver() ) return 2;

        roundsLeft -= 2;
    }
    return 0;
}