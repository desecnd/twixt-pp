#include "board/Board.hpp"
#include "game/Game.hpp"

#include "imgui.h"
#include "imgui-SFML.h"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/CircleShape.hpp>

#include <iostream>
#include <random>


void nothing();


int main() {
    Game game{};
    
    game.playGame();
}

void nothing() {
    sf::RenderWindow window(sf::VideoMode(640, 480), "Twixt-PP");
    window.setFramerateLimit(60);
    ImGui::SFML::Init(window);

    sf::CircleShape shape(100.f);
    shape.setFillColor(sf::Color::Green);

    render::Painter painter;
    Board board;

    sf::Clock deltaClock;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            ImGui::SFML::ProcessEvent(event);

            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        window.clear();
        window.draw(shape);
        ImGui::SFML::Render(window);

        painter.drawBoard(window, board);
        window.display();
    }

    ImGui::SFML::Shutdown();


}