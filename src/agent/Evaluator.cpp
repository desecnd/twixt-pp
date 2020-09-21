#include "Evaluator.hpp"

#include <sstream>

namespace agent {
    std::vector<std::vector<int>> Evaluator::calculateDistance(const Board& board, int testingPlayer) {

        std::queue<Position> Q;
        std::vector<std::vector<int>> dist(Board::kRows, std::vector<int>(Board::kCols, kInf));

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
                        && dist[u.row()][u.col()] == kInf ) {

                    Q.push(u);
                    dist[u.row()][u.col()] = dist[v.row()][v.col()] + 1;
                }
            }
        }

        return dist;
    }
    int Evaluator::simulateExploitation(Board board) {
        /// Simulates Greedy Exploitation if every player would greedly
        /// Go for an edge, and who would win 
        /// Takes advantage of blocking etc

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
        /// Returns difference of connected componenets 
        /// In respect to player 1 -> positive if player 2 has more 
        int ccp1 = countConnectedComponents(board, 1);
        int ccp2 = countConnectedComponents(board, 2);
        
        Score p1 { static_cast<Score>(ccp1) };
        Score p2 { static_cast<Score>(ccp2) };
        return p2 - p1;
    }

    int Evaluator::dijkstraMinimumLinks(const Board& board, int player) {
        /// Calculates Minimum Links remaining to connect 2 sides for player p

        struct Node {
            Position p;
            int d;

            Node(Position pp, int dd) : p(pp), d(dd) {}
        };

        struct Comparator {

            // compare by distance
            bool operator()(const Node& a, const Node& b) {
                if ( a.d == b.d ) {
                    if ( a.p.row() == b.p.row() ) return a.p.col() < b.p.col();
                    else return a.p.row() < b.p.row();
                }
                else return a.d < b.d;
            }
        };

        std::priority_queue<Node, std::vector<Node>, Comparator> pq;
        std::vector<std::vector<int>> dist(Board::kRows, std::vector<int>(Board::kCols, kInf));

        if ( player == 1 ) {
            for (int c = 0; c < Board::kCols; c++) {
                dist[0][c] = 0;
                pq.emplace(Position(0, c), 0);
            }
        }
        else if ( player == 2 ) {
            for (int r = 0; r < Board::kRows; r++) {
                dist[r][0] = 0;
                pq.emplace(Position(r, 0), 0);
            }
        }


        while ( pq.size() ) {
            Node n { pq.top() }; pq.pop();

            if ( dist[n.p.row()][n.p.col()] != n.d ) 
                continue;

            for (int directionInt = 0; directionInt < DIRECTIONS; directionInt++) {
                Direction direction { static_cast<Direction>(directionInt) };

                Position u { n.p + getVector(direction) };

                if ( board.possible(u, player) ) {
                    int w = 1;
                    if ( board.linkExist(n.p, direction, player) ) w = 0;

                    if ( board.pegOwner(u) != board.opponent(player) 
                        && board.linkPossible(n.p, direction, player) 
                        && n.d + w < dist[u.row()][u.col()] ) 
                    {
                        dist[u.row()][u.col()] = n.d + w;
                        pq.emplace(u, n.d + w);
                    }
                }
            }
        }

        int minDist { kInf };
        if ( player == 1 ) {
            for (int c = 0; c < Board::kCols; c++) 
                minDist = std::min(minDist, dist[Board::kRows - 1][c]);
        }
        else if ( player == 2 ) {
            for (int r = 0; r < Board::kRows; r++) 
                minDist = std::min(minDist, dist[r][Board::kCols - 1]);
        }

        return minDist;
    }

    Score Evaluator::minimumLinksHeuristic(const Board& board) {
        /// Returns difference between minimum links remaining in terms
        /// of first player
        /// if cut interval into range: [-10, 10]

        int minDistP1 { dijkstraMinimumLinks(board, 1) };
        int minDistP2 { dijkstraMinimumLinks(board, 2) };
        
        int difference { minDistP2 - minDistP1 };
        return std::min(std::max(difference, -10), 10);
    }

    Score Evaluator::edgeDistanceHeuristic(const Board& board) {
        auto distP1 = calculateDistance(board, 1);
        auto distP2 = calculateDistance(board, 2);

        Score score { 0 };
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

    Score Evaluator::pieceHeuristic(const Board& board) {
        Score val  { 0 };
        for (int r = 0; r < Board::kRows; r++) 
        for (int c = 0; c < Board::kCols; c++) {
            Position pos { r, c };
            if ( board.pegOwner(pos) == 0) continue;


            int vertical = std::min(r, Board::kRows - 1 - r);
            int horizontal = std::min(c, Board::kCols - 1 - c);

            val += (board.pegOwner(pos) == 1 ? 1 : -1) * std::min(horizontal, vertical);
        }
        return val;
    }

    Score Evaluator::exploitationHeuristic(const Board& board) {
        int winner = simulateExploitation(board);
        return static_cast<Score>(winner == 1 ? 1 : -1);
    }

    Score Evaluator::evaluateBoardScore(const Board& board) {
        // Score edgeHeur { edgeDistanceHeuristic(board) };
        int gameOver = board.isGameOver();

        if ( gameOver == 1 ) return kWin;
        else if ( gameOver == 2 ) return -kWin; 
        else { 
            int sureWinner = board.isGameSealed();
            if ( sureWinner == 1 ) return kSureWin;
            else if ( sureWinner == 2 ) return -kSureWin;

            Score expHeur { exploitationHeuristic(board) };
            Score ccHeur { connectedComponentsHeuristic(board) };
            Score pieceHeur { pieceHeuristic(board) };
            // Score edgeHeur { edgeWinnersHeuristic(board) };
            Score minLinkHeur { minimumLinksHeuristic(board) };
            // Score sum = expHeur * 100 + ccHeur * 20;
            Score sum = expHeur * 1000 + minLinkHeur * 100 + pieceHeur * 20 + ccHeur * 200;
            // Score sum = expHeur * 1000 + minLinkHeur * 100 + ccHeur * 200;
            return sum;
        }
    }

    std::string Evaluator::getRaport(const Board& board) {
        // Score edgeHeur { edgeDistanceHeuristic(board) };
        int gameOver = board.isGameOver();

        std::ostringstream os;

        if ( gameOver == 1 ) os << "= Win: " << kWin;
        else if ( gameOver == 2 ) os << "= Loose: " << kWin; 
        else { 
            int sureWinner = board.isGameSealed();
            if ( sureWinner == 1 ) os << "= Sealed Win: " << kSureWin;
            else if ( sureWinner == 2 ) os << "= Sealed Loose: " << -kSureWin;

            Score expHeur { exploitationHeuristic(board) };
            Score ccHeur { connectedComponentsHeuristic(board) };
            Score pieceHeur { pieceHeuristic(board) };
            Score edgeHeur { edgeWinnersHeuristic(board) };
            Score minLinkHeur { minimumLinksHeuristic(board) };

            os << " 1000*[expHeur:" << expHeur << "]";
            os << " + " << "200*[ccHeur:" << ccHeur << "]"; 
            os << " + " << "100*[minLinkHeur:" << minLinkHeur << "]";
            os << " = "; 
            // Score sum { expHeur * 100 + ccHeur * 20 };
            // Score sum { expHeur * 1000 + minLinkHeur * 100 + ccHeur * 200 };
            Score sum { expHeur * 1000 + minLinkHeur * 100 + pieceHeur * 20 + ccHeur * 200 + 50 * edgeHeur };

            os << sum;
        }
        return os.str();
    }
}