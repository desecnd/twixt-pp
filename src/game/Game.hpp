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
    std::vector<Board> history_;
    agent::Agent agent_;
    sf::RenderWindow window_;
    sf::Clock clock_;
    render::Painter painter_;

    Position cursor_;
    std::vector<agent::Stats> agentStats_;

    int player_perspective_; 

    std::mutex board_mutex_; 
public:
    Game() : rng_(), board_(), history_{ board_ }, agent_( rng_ ), 
        //window_(sf::VideoMode(640, 480), "Twixt-PP"),
        window_(sf::VideoMode(1920, 1080), "Twixt-PP"), 
        clock_(), painter_(), cursor_(), agentStats_{agent::Stats()}, player_perspective_(1)
        {
            window_.setFramerateLimit(30);
            ImGui::SFML::Init(window_);
        }

    ~Game() {
        ImGui::SFML::Shutdown();
    }

    bool makeMove(int player, Move move);

    void playGame(int player_perspective = 1);

    void playAgent();

    void draw();
    void drawImgui();

    void renderGameEditor();
    void renderAgent();
};