#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <chrono>
#include <utility>
#include <random>
#include <array>
#include <unordered_map>
#include <cmath>
#include <cassert>
#include <queue>

using hash_t = std::uint64_t;

const auto seed { 2501 };
// const auto seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
const int ROWS { 12 };
const int COLS { 12 };
const int PLAYERS { 2 };
const int DIRECTIONS { 8 };
const int inf { (int) 1e9 };
const float INF { 1e9 };

enum direction_t {
    right,
    sright,
    up,
    sup,
    left,
    sleft,
    down,
    sdown
};

inline direction_t opposite(direction_t dir) {
    return static_cast<direction_t>((static_cast<int>(dir) + DIRECTIONS/2 ) % DIRECTIONS);
}

inline direction_t rotate(direction_t dir) {
    return static_cast<direction_t>((static_cast<int>(dir) + 2) % DIRECTIONS);
}

struct Position {
    int row, col;    
    
    Position(int a, int b) : row(a), col(b) {}
    Position() : row{inf}, col{inf} {}
    
    Position& operator+=(const Position& rhs) { row += rhs.row; col += rhs.col; return *this;}
    friend Position operator+(Position lhs, const Position& rhs) { lhs += rhs; return lhs; };
    Position& operator-=(const Position& rhs) { row -= rhs.row; col -= rhs.col; return *this;}
    friend Position operator-(Position lhs, const Position& rhs) { lhs -= rhs; return lhs; };
    
    friend std::ostream& operator<<(std::ostream& out, const Position& pos) {
        out << "(" << pos.row << "," << pos.col << ")";
        return out;
    }
};

const std::array<Position, DIRECTIONS> dirVectors 
{{ {-1, 2}, { 1, 2}, {-2,-1}, {-2, 1}, { 1,-2}, {-1,-2}, {2, 1}, {2,-1} }};

Position rotate(const Position& pos) { return Position(-pos.col, pos.row); }

inline int index(const Position& pos) { return pos.row * COLS + pos.col; }
inline Position position(int index) { return {index / COLS, index % COLS }; }
inline int opponent(int player) { return 1 - player; }

struct ZobristKeys {
    hash_t pos_hash[ROWS * COLS][PLAYERS];
    hash_t dir_hash[ROWS * COLS][PLAYERS * DIRECTIONS];
    
    ZobristKeys() {
        std::mt19937_64 rng{ seed  };
        std::uniform_int_distribution<hash_t> distr{0u, std::numeric_limits<hash_t>::max()};
        
        for (int i = 0; i < ROWS * COLS; i++) {
            for (int p = 0; p < PLAYERS; p++) {
                pos_hash[i][p] = distr(rng);
                for (int d = 0; d < DIRECTIONS; d++)
                    dir_hash[i][p * DIRECTIONS + d] = distr(rng);
            }
        }
    }
    
    hash_t operator()(const Position& pos, int player) { return pos_hash[index(pos)][player]; }    
    hash_t operator()(const Position& pos, int player, direction_t dir) { return dir_hash[index(pos)][player * DIRECTIONS + dir]; }

} zobrist;


int hits = 0;

struct Board {
    static constexpr int COLLISION_LINKS { 9 };
    static std::unordered_map<hash_t, float> transpositionTable;

    using linkCollisions_t = std::array< std::pair<Position, direction_t> , COLLISION_LINKS >;
    static std::array<linkCollisions_t , DIRECTIONS> possibleCollisions; 

    hash_t hash;
    int currentPlayer;    
    std::array<std::uint8_t, ROWS * COLS> pawns;
    std::array<std::uint16_t, ROWS * COLS> links;
    std::array<int, PLAYERS> minMovesToWin;
    
    Board() : hash{ }, currentPlayer{ 0 }, pawns{ }, links{ }, minMovesToWin{{inf, inf}} {}

    bool inline linked(const Position& pos, direction_t dir, int player) const {
        return (links[index(pos)] & ( 1<<(DIRECTIONS * player + dir) ));
    }

    bool linkPossible(const Position& pos, direction_t dir, int player) {
        for ( auto p : possibleCollisions[dir] ) {
            if ( linked(pos + p.first, p.second, opponent(player)) ) 
                return false;
        }
        return true;
    }

    void setLink(const Position& pos, direction_t dir, int player) { 
        links[index(pos)] |= (1 << (DIRECTIONS * player + dir));
        links[index(pos + dirVectors[dir])] |= (1 << (DIRECTIONS * player + opposite(dir)));
        hash ^= zobrist(pos, player, dir);
        hash ^= zobrist(pos + dirVectors[dir], player, opposite(dir));
    }

    void setPawn(const Position& pos, int player) { 
        pawns[index(pos)] |= (1 << player);
        hash ^= zobrist(pos, player);
    }

    bool inline checkPawn(const Position& pos, int player) const {
        return (pawns[index(pos)] & (1 << player));
    }

    bool clearPawn(const Position& pos) const { return (pawns[index(pos)] == 0u);} 
    bool valid(const Position& pos) const { return (pos.row >= 0 && pos.row < ROWS && pos.col >= 0 && pos.col < COLS); }
    
    std::vector<Position> getMoves() {
        std::vector<Position> positions;
        
        
        int ioff = 0, joff = 0;
        if ( currentPlayer == 1 ) ioff = 1;
        else if ( currentPlayer == 0 ) joff = 1;
        
        /*
        for(int i = ioff; i < ROWS - ioff; i++)
        for(int j = joff; j < COLS - joff; j++)          
            if ( clearPawn({i, j})) 
                positions.emplace_back(i, j);
        */

        for(int i = ioff; i < ROWS - ioff; i++)
        for(int j = joff; j < COLS - joff; j++) 
        {
            Position pos { i, j };
            if ( !clearPawn(pos) ) continue;
            
            bool mayLink = false;
            for ( int directionInt = 0; directionInt < DIRECTIONS; directionInt++) 
            {

                direction_t direction { static_cast<direction_t>(directionInt) };
                Position current { pos + dirVectors[direction] };
                if ( valid(current) && checkPawn(current, currentPlayer) && linkPossible(pos, direction, currentPlayer) ) 
                    mayLink = true;
            }

            if ( mayLink ) positions.push_back(pos);
        }
        std::cerr << "Returning: " << positions.size() << " positions\n";
        assert(positions.size());
        return positions; 
    }
    
    void move(const Position& pos) {
        setPawn(pos, currentPlayer); 
  
        for (int directionInt = 0; directionInt < DIRECTIONS; directionInt++) {
            direction_t direction { static_cast<direction_t>(directionInt) };
            Position away { pos + dirVectors[direction] };

            if ( valid(away) && checkPawn(away, currentPlayer) ) {
                if ( linkPossible(pos, direction, currentPlayer) ) 
                    setLink(pos, direction, currentPlayer);
            }
        }

        calculateMinMovesRequiredToWin(currentPlayer);
        currentPlayer = opponent(currentPlayer);
    }

    void calculateMinMovesRequiredToWin(int player) {

        int minMoves= inf;
        std::vector<std::vector<int>> dp(ROWS, std::vector<int>(COLS, inf));

        if ( player == 0 ) 
        {
            for (int j = 0; j < COLS; j++) 
                dp[0][j] = (checkPawn({0, j}, player) ? 0 : 1);

            for (int i = 0; i < ROWS; i++) 
            for (int j = 0; j < COLS; j++) 
            {
				Position current { i, j };
				for (int directionInt = 0; directionInt < DIRECTIONS; directionInt++) 
                {
					direction_t direction { static_cast<direction_t>(directionInt) };
					Position target { current + dirVectors[direction] };

					if ( valid(target) && !checkPawn(target, opponent(player)) && linkPossible(current, direction, player) )
                    {
						int cost = (checkPawn(target, player) ? 0 : 1 );
						dp[target.row][target.col] = std::min(dp[target.row][target.col], dp[current.row][current.col] + cost);
					}
				}
			}

			for (int j = 0; j < COLS; j++) 
                minMoves = std::min(minMoves, dp[ROWS - 1][j]);
		}
		else if ( player == 1 ) 
        {
            for (int i = 0; i < ROWS; i++) 
                dp[i][0] = (checkPawn({i, 0}, player) ? 0 : 1);

            for (int j = 0; j < COLS; j++) 
            for (int i = 0; i < ROWS; i++)  
            {
				Position current { i, j };
				for (int directionInt = 0; directionInt < DIRECTIONS; directionInt++) 
                {
					direction_t direction { static_cast<direction_t>(directionInt) };
					Position target { current + dirVectors[direction] };

					if ( valid(target) && !checkPawn(target, opponent(player)) && linkPossible(current, direction, player) )
                    {
						int cost = (checkPawn(target, player) ? 0 : 1 );
						dp[target.row][target.col] = std::min(dp[target.row][target.col], dp[current.row][current.col] + cost);
					}
				}
			}
            for (int i = 0; i < ROWS; i++) 
                minMoves = std::min(minMoves, dp[i][COLS - 1]);
		}
        /*
        std::cerr << "\n";
        for ( auto &v : dp ) 
        {
            for ( auto x : v ) {
                if ( x == inf ) std::cerr << "x ";
                else std::cerr << x << " ";
            }
            std::cerr << "\n";
        }
        */

        minMovesToWin[player] = minMoves;
    }


    float evaluate() {
        if ( transpositionTable.find(hash) != transpositionTable.end() ) {
            hits++;
            return transpositionTable[hash];
        }

        int oppMovesLeft = minMovesToWin[opponent(currentPlayer)];
        int myMovesLeft = minMovesToWin[currentPlayer];
        float eval = 10 * oppMovesLeft - 5 * myMovesLeft;
        if ( oppMovesLeft == 0 ) eval = -INF;
        else if ( myMovesLeft == 0 ) eval = INF;

		transpositionTable[hash] = eval;
        return eval;
    }

    inline bool isGameOver() {
		return ( minMovesToWin[currentPlayer] == 0 || minMovesToWin[opponent(currentPlayer)] == 0 );
    }

    void debug() {
        std::cerr << "Game state hash: " << hash << "\n";
        std::cerr << "Eval: " << transpositionTable[hash] << "\n";
        std::cerr << "Current player: " << currentPlayer << "\n";
        for (int i = 0; i < ROWS; i++) {
            
            for (int j = 0; j < COLS; j++) {
                if ( clearPawn({i, j}) ) std::cerr << ". ";
                else if ( checkPawn({i, j}, 0) ) std::cerr << "0 ";
                else std::cerr << "1 "; 

            }
            std::cerr << "\n";
        }


    }
};

std::array<Board::linkCollisions_t , DIRECTIONS > Board::possibleCollisions { 
[]{
    std::array<linkCollisions_t , DIRECTIONS > collisions;
    linkCollisions_t rightCollisions {{ 
        { {-1, 1}, sright },
        { {-1, 1}, down },
        { {-1, 1}, sdown },
        { {-1, 0}, sright },
        { {-1, 0}, down },
        { { 0, 1}, up },
        { { 0, 1}, sup },
        { { 0, 1}, sleft },
        { { 0, 2}, up } 
    }};

    linkCollisions_t srightCollisions {{ 
        { { 1, 1}, right },
        { { 1, 1}, sup },
        { { 1, 1}, up },
        { { 1, 0}, right },
        { { 1, 0}, sup },
        { { 0, 1}, sdown },
        { { 0, 1}, down },
        { { 0, 1}, left },
        { { 0, 2}, sdown } 
    }};

    std::copy(rightCollisions.begin(), rightCollisions.end(), collisions[right].begin());
    std::copy(srightCollisions.begin(), srightCollisions.end(), collisions[sright].begin());
    
    for (int directionInt = 2; directionInt < DIRECTIONS; directionInt+=2) {
        for (int s = 0; s < 2; s++ ) {
            direction_t last { static_cast<direction_t>(directionInt + s - 2) };
            direction_t current = static_cast<direction_t>(directionInt + s);
        
            for (int i = 0; i < COLLISION_LINKS; i++) {
                collisions[current][i].first = rotate(collisions[last][i].first);    
                collisions[current][i].second = rotate(collisions[last][i].second);
            }
        } 
    }
    return collisions;
}()
};

std::unordered_map<hash_t, float> Board::transpositionTable{};

float ABNegamax(Board& board, int maxDepth, int depth, float alpha, float beta, Position& bestMove) {
    if ( depth == maxDepth || board.isGameOver() ) 
        return board.evaluate(); 

    float bestScore = -INF;
    auto moves { board.getMoves() };
    for ( auto move : moves ) {
        Board newBoard(board);
        newBoard.move(move);

        Position tempMove{};
        float recursedScore { ABNegamax(newBoard, maxDepth, depth + 1, -beta, -std::max(alpha, bestScore), tempMove) };
        float currentScore = -recursedScore;

        if ( currentScore > bestScore ) {
            bestScore = currentScore;
            bestMove = move;
        }

        if ( bestScore >= beta )
            break;
    }

    return bestScore;
}

/*
int alphabeta(Board& board, int alpha, int beta, int depth, int maxDepth, Position& bestMove, int player, int &evalCount) {
    if ( depth == maxDepth || board.isGameOver() ) {
        bestMove = {-1, -1};
        evalCount++;
        return board.evaluate(player);
    }

    int bestScore;
    if ( board.currentPlayer == player ) bestScore = -inf;
    else bestScore = inf;
    
    for ( auto move : board.getMoves() ) {
        Board newBoard { board };
        newBoard.move(move);
        
        Position currentMove = move;
        int currentScore = alphabeta(newBoard, alpha, beta, depth + 1, maxDepth, currentMove, player, evalCount);
        
        if ( board.currentPlayer == player) {
            if ( currentScore > bestScore ) {
                bestScore = currentScore;
                bestMove = move;
                alpha = currentScore;
            }
        }
        else {
            if ( currentScore < bestScore ) {
                bestScore = currentScore;
                bestMove = move;
                beta = currentScore;
            }
        }  
        
        if ( alpha >= beta ) break;
    }
    
    return bestScore;
}*/

std::string toString(const Position& pos) {
    std::string out(1, 'A' + pos.col); 
    out += std::to_string(pos.row + 1);
    return out;
}

Position toPosition(const std::string& out) {
    Position pos {};
    pos.col = out[0] - 'A';
    pos.row = std::stoi(out.substr(1, out.size() - 1)) - 1;
    return pos;
}

Position getBestMove(Board &board, int maxDepth) {
    Position move{};
    float bestScore { ABNegamax(board, maxDepth, 0, -INF, INF, move) };
    return move;
}

int M[ROWS][COLS];

void printMark() {
    for (int i = 0; i < ROWS; i++) {
    for (int j = 0; j < COLS; j++) {
        if ( M[i][j] == 0 ) std::cerr << ". ";
        else std::cerr << M[i][j] << " ";
    }
        std::cerr << "\n";
    }

}
void mark(const Position& pos, int x) {
    M[pos.row][pos.col] = x;
}

void test() 
{
    // sequence: my: I3, J5, K7, J9
    // his: E6, G5, I6
    Board board;
    std::queue<std::string> myPlays({ "E6" });
    std::queue<std::string> enemyPlays;
    /*
    while ( myPlays.size() && enemyPlays.size() ) 
    { 
        board.move(toPosition(myPlays.front())); myPlays.pop();
        board.move(toPosition(enemyPlays.front())); enemyPlays.pop();  
    }
    if ( myPlays.size() ) { board.move(toPosition(myPlays.front())); myPlays.pop(); }

    board.evaluate(1);
    board.debug();
    
    std::cerr << "Min req enemy: " << board.minMovesRequiredToWin(1) << "\n";
    std::cerr << "Min req me: " << board.minMovesRequiredToWin(0) << "\n";
    */

    Position pos { toPosition("E6") };
    mark(pos, 1);
    for (int directionInt = 0; directionInt < DIRECTIONS; directionInt++) {
        direction_t direction { static_cast<direction_t>(directionInt)};

        std::cerr << "Checking direction: " << directionInt << "\n";

        Position current { pos + dirVectors[direction] };
        mark(current, 2);

        int link = 0;
        for (const auto &lc : Board::possibleCollisions[direction]) {
            Position next { pos + lc.first };
            Position other { next + dirVectors[lc.second] };

            mark(next, 3);
            mark(other, 4);
            board.setLink(next, lc.second, 0);
            std::cerr << "link #" << ++link << ", link poss?: " << std::boolalpha << board.linkPossible(pos, direction, 1) << "\n";

            printMark();
            std::cerr << "\n";
            mark(next, 0);
            mark(other, 0);

        }

        mark(current, 0);
    }

}

void readTrashInput() {
    int numYourPegs; // The number of pegs you have on the board.
    std::cin >> numYourPegs; std::cin.ignore();
    for (int i = 0; i < numYourPegs; i++) {
        std::string yourPeg; // One of your pegs.
        std::cin >> yourPeg; std::cin.ignore();
    }
    int numYourSegments; // The number of segments you have on the board.
    std::cin >> numYourSegments; std::cin.ignore();
    for (int i = 0; i < numYourSegments; i++) {
        std::string yourSegPeg1; // The first end of one of your segments.
        std::string yourSegPeg2; // The second end of one of your segments.
        std::cin >> yourSegPeg1 >> yourSegPeg2; std::cin.ignore();
    }
    int numHisPegs; // The number of pegs your opponent has on the board.
    std::cin >> numHisPegs; std::cin.ignore();
    for (int i = 0; i < numHisPegs; i++) {
        std::string hisPeg; // One of his pegs.
        std::cin >> hisPeg; std::cin.ignore();
    }
    int numHisSegments; // The number of segments of your opponent.
    std::cin >> numHisSegments; std::cin.ignore();
    for (int i = 0; i < numHisSegments; i++) {
        std::string hisSegPeg1; // The first end of one of his segments.
        std::string hisSegPeg2; // The second end of one of his segments.
        std::cin >> hisSegPeg1 >> hisSegPeg2; std::cin.ignore();
    }
}

int main()
{
    using std::cerr;
    using std::cin;
    using std::cout;
    using std::endl;

    //test();

    int me = 1;
    Board board;
    Position first { 6, 10 };
    Position second { 6, 2 };

    for (int turn = 1; !board.isGameOver() ; turn++) {
        std::string hisLastPeg; // Your opponent's last peg (B3, or H10 for example), or FIRST, or SWAP.
        cin >> hisLastPeg; cin.ignore();

        // readTrashInput();
        
        // cerr << hisLastPeg << "\n";
        Position move;

        int deep = (turn >= 8 ? 2 : 2);

        if ( turn == 1 ) {
            if ( hisLastPeg == "FIRST") {
                me = 0;
                move = first;
            }
            else {
                me = 1;
                board.move(toPosition(hisLastPeg));
                first = Position(first.col, first.row);
                second = Position(second.col, second.row);
                if ( hisLastPeg != toString(first)) move = first;
                else move = second;
            }
        } 
        else if ( hisLastPeg == "SWAP") {
            board = Board{};
            board.currentPlayer = 1;
            board.move({ first.col, first.row });
            move = second;
        } else {
            board.move(toPosition(hisLastPeg));
            if ( board.isGameOver() ) break;

            /*
            auto positions { board.getMoves() };
            std::cerr << "Positions:\n";
            for ( const Position& pos : positions ) {
                std::cerr << toString(pos) << " ";
            }
            std::cerr << "\n";
            */
            
            int evalCount = 0;
            
            move = getBestMove(board, 3);
            //cerr << "Move score: " << score << ", move: " << move << ", evals: " << evalCount << ", transpositions: " << hits << "\n";
        }

        board.move(move);
        cout << toString(move) << endl;

        // board.debug();
        
    }
}
