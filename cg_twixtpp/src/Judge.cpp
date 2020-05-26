#include "Judge.hpp"

int Judge::playGame(Player& p1, Player& p2, bool debug = false) {
    Move m1 { "FIRST" };
    Move m2 { "FIRST" };

    int roundsLeft { 12 * 12 - 4 };
    while ( roundsLeft > 0 ) {
        m1 = p1.getMove(m2);
        applyMove(board_, m1);
        if ( board_.isGameOver() ) return 1;

        m2 = p2.getMove(m1);
        applyMove(board_, m2);
        if ( board_.isGameOver() ) return 2;

        roundsLeft -= 2;
    }
    return 0;
}
