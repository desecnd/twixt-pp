#include "Evaluator.hpp"

namespace agent {
    std::vector<std::vector<int>> Evaluator::calculateDistance(const Board& board, int testingPlayer) {
        const int inf = 1e9;

        std::queue<Position> Q;
        std::vector<std::vector<int>> dist(Board::kRows, std::vector<int>(Board::kCols, inf));

        for (int r = 0; r < Board::kRows; r++) 
        for (int c = 0; c < Board::kCols; c++) {
            Position pos { r, c };
            int player = board.pegOwner(pos);

            if ( player == testingPlayer ) {
                Q.push(pos);
                dist[pos.row()][pos.col()] = 0;
            }
        }

        while ( Q.size() ) {
            Position v = Q.front(); Q.pop();

            for (int directionInt = 0; directionInt < DIRECTIONS; directionInt++) {
                Direction direction { static_cast<Direction>(directionInt) };

                Position u { v + getVector(direction) };

                if ( board.possible(u, testingPlayer) 
                    && board.linkPossible(v, direction, testingPlayer) 
                        && dist[u.row()][u.col()] == inf ) {

                    Q.push(u);
                    dist[u.row()][u.col()] = dist[v.row()][v.col()] + 1;
                }
            }
        }

        return dist;
    }
    int Evaluator::simulateExploitation(Board board) {
        // std::cerr << "Current PLayer: " << board.currentPlayer() << ";";
        std::queue<Position> Q;

        for (int r = 0; r < Board::kRows; r++) 
        for (int c = 0; c < Board::kCols; c++) {
            Position pos { r, c };
            int player = board.pegOwner(pos);
            if ( player && player == board.currentPlayer()  ) 
                Q.push(pos);
        }

        for (int r = 0; r < Board::kRows; r++) 
        for (int c = 0; c < Board::kCols; c++) {
            Position pos { r, c };
            int player = board.pegOwner(pos);
            if ( player && player != board.currentPlayer()  ) 
                Q.push(pos);
        }

        while ( Q.size() ) {
            Position v = Q.front(); Q.pop();
            int player = board.pegOwner(v);
            board.setPlayer(player);

            for (int directionInt = 0; directionInt < DIRECTIONS; directionInt++) {
                Direction direction { static_cast<Direction>(directionInt) };

                Position u { v + getVector(direction) };

                if ( board.possible(u, player) 
                    && board.linkPossible(v, direction, player) && !board.pegOwner(u) ) {

                    Q.push(u);
                    board.markPeg(u, player);
                    board.markLink(v, direction, player);
                }
            }
        }


        // printBoard(std::cout, board);

        return board.isGameOver();
    }

    std::vector<std::vector<int>> Evaluator::getEdgeWinners(Board board) {
        std::queue<Position> Q;

        for (int r = 0; r < Board::kRows; r++) 
        for (int c = 0; c < Board::kCols; c++) {
            Position pos { r, c };
            int player = board.pegOwner(pos);
            if ( player && player == board.currentPlayer()  ) 
                Q.push(pos);
        }

        for (int r = 0; r < Board::kRows; r++) 
        for (int c = 0; c < Board::kCols; c++) {
            Position pos { r, c };
            int player = board.pegOwner(pos);
            if ( player && player != board.currentPlayer()  ) 
                Q.push(pos);
        }

        while ( Q.size() ) {
            Position v = Q.front(); Q.pop();
            int player = board.pegOwner(v);
            board.setPlayer(player);

            for (int directionInt = 0; directionInt < DIRECTIONS; directionInt++) {
                Direction direction { static_cast<Direction>(directionInt) };

                Position u { v + getVector(direction) };

                if ( board.possible(u, player) 
                    && board.linkPossible(v, direction, player) && !board.pegOwner(u) ) {

                    Q.push(u);
                    board.takePeg(u, true);
                }
            }
        }

        std::vector<std::vector<int>> winner(Board::kRows, std::vector<int>(Board::kCols, 0));

        for (int r = 0; r < Board::kRows; r++)
        for (int c = 0; c < Board::kCols; c++) {
            Position pos { r, c };
            winner[r][c] = board.pegOwner(pos);
        }

        return winner;
    }

    void Evaluator::dfs(const Board& board, std::vector<std::vector<int>>& vis, Position pos, int player) {
        vis[pos.row()][pos.col()] = 1;

        for (int directionInt = 0; directionInt < DIRECTIONS; directionInt++) {
            Direction direction { static_cast<Direction>(directionInt) };

            Position next { pos + getVector(direction) };
            if ( board.possible(next, player) && !vis[next.row()][next.col()] 
                && board.pegOwner(next) == player && board.linkExist(pos, direction, player))
                dfs(board, vis, next, player);
        }
    }

    int Evaluator::countConnectedComponents(const Board& board, int testingPlayer) {
        std::vector<std::vector<int>> vis(Board::kRows, std::vector<int>(Board::kCols, 0));
        int cc = 0;

        for (int r = 0; r < Board::kRows; r++) 
        for (int c = 0; c < Board::kCols; c++) {
            Position pos { r, c };
            if ( board.pegOwner(pos) == testingPlayer && !vis[r][c]) {
                dfs(board, vis, pos, testingPlayer);
                cc++;
            }
        }

        return cc;
    }

    Score Evaluator::edgeWinnersHeuristic(const Board& board) {
        auto winner = getEdgeWinners(board);

        int add{ 0 };
        int p1Wins{ 0 };
        int p2Wins{ 0 };

        for (int r = 1; r < Board::kRows - 1; r++ ) {
            if( winner[r][0] ) add = 1; 
        }

        p2Wins += add; 
        add = 0;
        for (int r = 1; r < Board::kRows - 1; r++ ) {
            if ( winner[r][Board::kCols - 1] ) add = 1;
        }
        p2Wins += add; 
        add = 0;

        for (int c = 1; c < Board::kCols - 1; c++ ) {
            if ( winner[0][c] ) add = 1;
        }
        p1Wins += add;
        add = 0;

        for (int c = 1; c < Board::kCols - 1; c++ ) {
            if ( winner[Board::kRows - 1][c] ) add = 1;
        }
        p1Wins += add;
        add = 0;


        /*
        std::cerr << "EXPHEUR:{{";
        for (int r = 1; r < Board::kRows - 1; r++ ) 
            std::cerr << winner[r][0] << " ";
        std::cerr << "}{";
        for (int r = 1; r < Board::kRows - 1; r++ ) 
            std::cerr << winner[r][Board::kCols - 1] << " ";
        std::cerr << "}{";
        for (int c = 1; c < Board::kCols - 1; c++ ) 
            std::cerr << winner[0][c] << " ";
        std::cerr << "}{";
        for (int c = 1; c < Board::kCols - 1; c++ ) 
            std::cerr << winner[Board::kRows - 1][c] << " ";
        std::cerr << "}}";
        */

        Score out { static_cast<Score>(p1Wins - p2Wins) };
        return out;
    }

    Score Evaluator::connectedComponentsHeuristic(const Board& board) {
        int ccp1 = countConnectedComponents(board, 1);
        int ccp2 = countConnectedComponents(board, 2);
        
        Score p1 { static_cast<Score>(ccp1) };
        Score p2 { static_cast<Score>(ccp2) };
        return p2 - p1;
    }

    Score Evaluator::edgeDistanceHeuristic(const Board& board) {
        auto distP1 = calculateDistance(board, 1);
        auto distP2 = calculateDistance(board, 2);

        Score score { 0.0 };
        for (int r = 1; r < Board::kRows - 1; r++ ) {
            float diff1 = distP1[r][0] - distP2[r][0];
            float diff2 = distP1[r][Board::kCols - 1] - distP2[r][Board::kCols - 1];

            score -= diff1 * diff1;
            score -= diff2 * diff2;
        }

        for (int c = 1; c < Board::kCols - 1; c++ ) {
            float diff1 = distP1[0][c] - distP2[0][c];
            float diff2 = distP1[Board::kRows - 1][c] - distP2[Board::kRows - 1][c];

            score -= diff1 * diff1;
            score -= diff2 * diff2;
       } 
        return score;

    }

    Score Evaluator::exploitationHeuristic(const Board& board) {
        int winner = simulateExploitation(board);
        return static_cast<Score>(winner == 1 ? 1 : -1);
    }

    Score Evaluator::evaluateBoardScore(const Board& board) {
        // Score edgeHeur { edgeDistanceHeuristic(board) };
        int gameOver = board.isGameOver();

        if ( gameOver == 1 ) return 1e9;
        else if ( gameOver == 2 ) return -1e9; 
        else { 
            Score expHeur { exploitationHeuristic(board) };
            Score ccHeur { connectedComponentsHeuristic(board) };
            Score sum = expHeur * 100 + ccHeur;
            return sum;
        }
    }
}