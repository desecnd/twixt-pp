#include "Painter.hpp"

#include "imgui.h"
#include "imgui-SFML.h"

#include <cassert>

namespace render {

    void Painter::drawBoard(sf::RenderWindow& window, const Board& board) {
        assert(window.isOpen());

        auto boardSize = calcBoardSize(window.getSize());
        float windowVertical = boardSize.x;
        float windowHorizontal = boardSize.y;

        auto distances = calcDistances(window.getSize());
        float horizontalDistance = distances.x; 
        float verticalDistance = distances.y;

        float pegRadius = calcPegRadius(window.getSize());
        float lineThickness = 5.f;

        // draw lines
        {
            sf::Vector2f firstPos, secondPos; 
            firstPos = sf::Vector2f(1.5f * verticalDistance, 1.5f * horizontalDistance);
            secondPos = sf::Vector2f(windowHorizontal - 1.5f * verticalDistance, 1.5f * horizontalDistance);
            
            window.draw(sfLine(firstPos, secondPos, colors[1], lineThickness));

            firstPos = sf::Vector2f(1.5f * verticalDistance, windowVertical - 1.5f * horizontalDistance);
            secondPos = sf::Vector2f(windowHorizontal - 1.5f * verticalDistance, windowVertical - 1.5f * horizontalDistance);
            window.draw(sfLine(firstPos, secondPos, colors[1], lineThickness));

            firstPos = sf::Vector2f(1.5f * verticalDistance, 1.5f * horizontalDistance);
            secondPos = sf::Vector2f(1.5f * verticalDistance, windowVertical - 1.5f * horizontalDistance);
            window.draw(sfLine(firstPos, secondPos, colors[2], lineThickness));

            firstPos = sf::Vector2f(windowHorizontal - 1.5f * verticalDistance, 1.5f * horizontalDistance);
            secondPos = sf::Vector2f(windowHorizontal - 1.5f * verticalDistance, windowVertical - 1.5f * horizontalDistance);
            window.draw(sfLine(firstPos, secondPos, colors[2], lineThickness));
        }

        // draw pegs
        for (int row = 0; row < Board::kRows; row++)
        for (int col = 0; col < Board::kCols; col++) {
            Position pos { row, col };

            sf::CircleShape peg(pegRadius, 30ul);
            peg.setOrigin(peg.getRadius(), peg.getRadius());
            peg.setFillColor(colors[board.pegOwner(pos)]);
            sf::Vector2f newPos { (col + 1) * horizontalDistance, (row + 1) * verticalDistance };
            peg.setPosition(newPos);
            window.draw(peg);
        }

        // draw links
        for (int row = 0; row < Board::kRows; row++)
        for (int col = 0; col < Board::kCols; col++) {
            Position pos { row, col };
            int pegOwner = board.pegOwner(pos);
            if ( pegOwner == 0 ) continue;

            for (int directionInt = 0; directionInt < DIRECTIONS; directionInt++) {
                Direction direction { static_cast<Direction>(directionInt) };
                Position other { pos + getVector(direction) };

                if ( board.linkExist(pos, direction, pegOwner) ) {
                    sf::Vector2f firstPos { (pos.col() + 1) * horizontalDistance, (pos.row() + 1) * verticalDistance };
                    sf::Vector2f secondPos { (other.col() + 1) * horizontalDistance, (other.row() + 1) * verticalDistance };

                    sf::VertexArray line(sf::LinesStrip, 2);
                    line[0].position = firstPos;
                    line[1].position = secondPos;
                    line[1].color = line[0].color = colors[pegOwner];

                    window.draw(line);
                }
            }
        }
    }

    void Painter::drawDebug(sf::RenderWindow& window, const agent::Agent& agent) { 
        ImGui::Begin("New Panel");
        ImGui::Button("Pretty Button");
        ImGui::End();
        ImGui::SFML::Render(window);
    }

    Position Painter::getLogicalPosition(const sf::RenderWindow& window, const sf::Vector2i& pos) {
        auto boardSize = calcBoardSize(window.getSize());
        float windowVertical = boardSize.x;
        float windowHorizontal = boardSize.y;

        auto distances = calcDistances(window.getSize());
        float verticalDistance = distances.x;
        float horizontalDistance = distances.y; 

        float pegRadius = calcPegRadius(window.getSize());

        float ypos = pos.y;
        float xpos = pos.x;

        for (int row = 0; row < Board::kRows; row++) 
        for (int col = 0; col < Board::kCols; col++) {
            float ypeg = (row + 1) * horizontalDistance;
            float xpeg = (col + 1) * verticalDistance;

            float xdiff = xpeg - xpos;
            float ydiff = ypeg - ypos;

            if ( xdiff * xdiff + ydiff * ydiff < pegRadius * pegRadius )
                return { row, col };
        }
        return { -1, -1 };
    }
}