#pragma once

#include "board/Board.hpp"
#include "agent/Agent.hpp"

#include <SFML/Graphics.hpp>

class sfLine : public sf::Drawable {
public:
    sfLine(const sf::Vector2f& point1, const sf::Vector2f& point2, sf::Color color = sf::Color::Yellow, float thic = 5.f):
        color(color), thickness(thic) {
        sf::Vector2f direction = point2 - point1;
        sf::Vector2f unitDirection = direction/std::sqrt(direction.x*direction.x+direction.y*direction.y);
        sf::Vector2f unitPerpendicular(-unitDirection.y,unitDirection.x);

        sf::Vector2f offset = (thickness/2.f)*unitPerpendicular;

        vertices[0].position = point1 + offset;
        vertices[1].position = point2 + offset;
        vertices[2].position = point2 - offset;
        vertices[3].position = point1 - offset;

        for (int i=0; i<4; ++i)
            vertices[i].color = color;
    }

    void draw(sf::RenderTarget &target, sf::RenderStates states) const {
        target.draw(vertices,4,sf::Quads);
    }
private:
    sf::Vertex vertices[4];
    float thickness;
    sf::Color color;
};

namespace render {
    class Painter {
    private:

        sf::Vector2f calcBoardSize(const sf::Vector2u& windowSize) const { 
            int minValue = std::min(windowSize.x, windowSize.y);
            return sf::Vector2f(minValue, minValue);
        }

        sf::Vector2f calcDistances(const sf::Vector2u& windowSize) const {
            auto boardSize { calcBoardSize(windowSize) };
            return sf::Vector2f(boardSize.x / (Board::kCols + 1), boardSize.y / (Board::kRows + 1));
        }

        float calcPegRadius(const sf::Vector2u& windowSize) const {
            auto distances { calcDistances(windowSize) };
            return std::min(distances.x, distances.y) / 5;
        }

    public:
        const std::array<sf::Color, 3> colors {{ 
            sf::Color(204, 153, 0), sf::Color(204, 0, 0), sf::Color(51, 51, 255)
        }};

        Painter() = default;
        void drawBoard(sf::RenderWindow& window, const Board& board);
        Position getLogicalPosition(const sf::RenderWindow& window, const sf::Vector2i& position);
    };

}
