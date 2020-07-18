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

class Game {
private:
    std::mt19937_64 rng_;
    Board board_;
    std::stack<Board> history_;
    agent::Agent agent_;
    sf::RenderWindow window_;
    sf::Clock clock_;
    render::Painter painter_;
public:
    Game() : rng_(), board_(), history_(), agent_( rng_ ), 
        //window_(sf::VideoMode(640, 480), "Twixt-PP"),
        window_(sf::VideoMode(1920, 1080), "Twixt-PP"), 
        clock_(), painter_()
        {
            window_.setFramerateLimit(60);
            ImGui::SFML::Init(window_);
            history_.push(board_);
        }

    ~Game() {
        ImGui::SFML::Shutdown();
    }

    void playGame(bool playerTurn = true);
    void draw();
};