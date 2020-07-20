#include "Game.hpp"

#include <thread>
#include <future>

void Game::draw() {
    window_.clear(sf::Color(61, 61, 92));
    painter_.drawBoard(window_, board_);

    ImGui::SFML::Update(window_, clock_.restart());
    painter_.drawDebug(window_, agent_);
    window_.display();
}


void Game::playAgent() {
    bool game_on = true;
    while ( game_on ) { 
        if ( board_mutex_.try_lock() )  {
            if (board_.currentPlayer() == agent_.perspective() ) {
                auto agent_move = agent_.getMove(board_); 
                if ( !applyMove(board_, agent_move) ) {
                    board_mutex_.unlock();
                    return;
                }
                history_.push(board_);

                if ( board_.isGameOver() )
                    game_on = false;
            }
            board_mutex_.unlock();
        }
    }
}

void Game::playGame(int player_perspective) {
    player_perspective_ = player_perspective;
    current_player_ = board_.currentPlayer();
    agent_.setPerspective(board_.opponent(player_perspective_));

    std::thread agent_thread(&Game::playAgent, this);

    while (window_.isOpen()) {

        sf::Event event;
        while (window_.pollEvent(event)) {
            ImGui::SFML::ProcessEvent(event);

            if (event.type == sf::Event::Closed) {
                window_.close();
            }

            else if (event.type == sf::Event::MouseButtonPressed) {
                sf::Vector2i position = sf::Mouse::getPosition(window_);

                std::lock_guard<std::mutex> lock(board_mutex_);
                if ( board_.currentPlayer() == player_perspective_ ) {
                    Position pos { painter_.getLogicalPosition(window_, position) };
                    if ( board_.possible(pos, player_perspective_) ) {
                        Move my_move = posToMove(pos);
                        applyMove(board_, my_move);
                        history_.push(board_);
                    }
                }
            }
        }

        if ( window_.isOpen() ) {
            draw();
        }
    }

    agent_thread.join();
}