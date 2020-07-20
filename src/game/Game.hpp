#pragma once

#include "board/Board.hpp"
#include "agent/Agent.hpp"
#include "render/Painter.hpp"

#include "Move.hpp"
#include "Player.hpp"

#include <SFML/Window.hpp>

#include "imgui.h" 
#include "imgui-SFML.h"

#include <stack>
#include <random>
#include <thread>
#include <mutex>

class Game {
private:
    std::mt19937_64 rng_;
    Board board_;
    std::stack<Board> history_;
    agent::Agent agent_;
    sf::RenderWindow window_;
    sf::Clock clock_;
    render::Painter painter_;

    int player_perspective_; 
    int current_player_;

    std::mutex board_mutex_; 
public:
    Game() : rng_(), board_(), history_(), agent_( rng_ ), 
        window_(sf::VideoMode(640, 480), "Twixt-PP"),
        //window_(sf::VideoMode(1920, 1080), "Twixt-PP"), 
        clock_(), painter_()
        {
            window_.setFramerateLimit(60);
            ImGui::SFML::Init(window_);
            history_.push(board_);
        }

    ~Game() {
        ImGui::SFML::Shutdown();
    }

    int opponent(int player) { return 3 - player; }
    void playGame(int player_perspective = 1);
    void playAgent();
    void draw();
};