#include "Game.hpp"

void Game::draw() {
    window_.clear();
    painter_.drawBoard(window_, board_);

    ImGui::SFML::Update(window_, clock_.restart());
    painter_.drawDebug(window_, agent_);
    window_.display();
}

void Game::playGame(bool playerTurn) {
    Move m1, m2;

    while (window_.isOpen()) {
        sf::Event event;

        if ( !playerTurn ) {
            m2 = agent_.getMove(board_); 
            applyMove(board_, m2);
            history_.push(board_);
            playerTurn = true;
        }

        draw();

        // get Human move
        while (window_.pollEvent(event)) {
            ImGui::SFML::ProcessEvent(event);

            if (event.type == sf::Event::Closed) {
                window_.close();
            }
            else if (event.type == sf::Event::MouseButtonPressed) {
                sf::Vector2i position = sf::Mouse::getPosition(window_);
                std::cerr << position.x << " " << position.y << "\n";

                Position pos { painter_.getLogicalPosition(window_, position) };
                std::cerr << pos << "\n";
            }
        }
    }
}