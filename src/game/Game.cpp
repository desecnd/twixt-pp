#include "Game.hpp"

#include <thread>
#include <future>

bool Game::makeMove(int player, Move move) {
    assert(board_.currentPlayer() == player);
    bool validMove = applyMove(board_, move);
    history_.push_back(board_);
    return validMove;
}

void Game::draw() {
    window_.clear(sf::Color(61, 61, 92));
    painter_.drawBoard(window_, board_);
    drawImgui();
    window_.display();
}

// Demonstrate using the low-level ImDrawList to draw custom shapes.
void Game::renderAgent()
{
    agent::Stats lastStats;
    if ( agentStats_.size() > 0 ) lastStats = agentStats_.back();

    ImGui::Begin("Agent Debug");

    // Tip: If you do a lot of custom rendering, you probably want to use your own geometrical types and benefit of
    // overloaded operators, etc. Define IM_VEC2_CLASS_EXTRA in imconfig.h to create implicit conversions between your
    // types and ImVec2/ImVec4. Dear ImGui defines overloaded operators but they are internal to imgui.cpp and not
    // exposed outside (to avoid messing with your types) In this example we are not using the maths operators!
    // ImDrawList* draw_list = ImGui::GetWindowDrawList();

    if (ImGui::BeginTabBar("##TabBar"))
    {
        if (ImGui::BeginTabItem("Evaluation Fuction"))
        {

            // Draw a bunch of primitives

            static float side_length = 18.f;
            static float break_length = 5.f; 
            static float alpha = 1.0f;
            static ImVec4 col_good = ImVec4(0.5f, .80f, 0.3f, alpha);
            static ImVec4 col_bad = ImVec4(0.9f, 0.2f, 0.2f, alpha);
            static ImVec4 col_inf = ImVec4(0.7f, 0.7f, 0.7f, alpha);
            static ImVec4 col_neg_inf = ImVec4(0.5f, 0.5f, 0.5f, alpha);
            static ImVec4 col_win = ImVec4(0.85f, 0.85f, 0.2f, alpha);
            static ImVec4 col_neg_win = ImVec4(0.85, 0.2f, 0.85f, alpha);
            static ImVec4 col_sure_win = ImVec4(0.85f, 0.85f, 0.2f, alpha/2);
            static ImVec4 col_neg_sure_win = ImVec4(0.85, 0.2f, 0.85f, alpha/2);

            if (ImGui::CollapsingHeader("Settings")) {
                ImGui::Text("Variables");
                ImGui::DragFloat("Side Lenght", &side_length, 0.2f, 2.0f, 72.0f, "%.0f");
                ImGui::DragFloat("Distance", &break_length, 0.05f, 1.0f, 8.0f, "%.02f");

                ImGui::Text("Color Pallete");
                ImGui::ColorEdit4("Fitness Good", &col_good.x);
                ImGui::ColorEdit4("Fitness Bad", &col_bad.x);
                ImGui::ColorEdit4("Infinity", &col_inf.x);
                ImGui::ColorEdit4("Negative Infinity", &col_neg_inf.x);
                ImGui::ColorEdit4("Winning", &col_win.x);
                ImGui::ColorEdit4("Loosing", &col_neg_win.x);
                ImGui::ColorEdit4("Determined Win", &col_sure_win.x);
                ImGui::ColorEdit4("Determined Loose", &col_neg_sure_win.x);
            }

            if (ImGui::CollapsingHeader("Heat Maps")) {

                ImGui::Columns(2);
                ImGui::Text("Fitness Heat Map");
                for (int row = 0; row < Board::kRows; row++)
                for (int col = 0; col < Board::kCols; col++)
                {
                    int id = row * Board::kCols + col;
                    agent::Score score = lastStats.positionScore[row][col];
                    ImU32 color;


                    switch ( score ) {
                        case agent::kInf: color = ImGui::GetColorU32(col_inf); break;
                        case -agent::kInf: color = ImGui::GetColorU32(col_neg_inf); break;
                        case agent::kWin: color = ImGui::GetColorU32(col_win); break;
                        case -agent::kWin: color = ImGui::GetColorU32(col_neg_win); break;
                        case agent::kSureWin: color = ImGui::GetColorU32(col_sure_win); break;
                        case -agent::kSureWin: color = ImGui::GetColorU32(col_neg_sure_win); break;
                        default: {
                            float width = lastStats.bestScore - lastStats.worstScore;
                            float percent = float(score - lastStats.worstScore) / width;

                            float r = col_bad.x + percent * (col_good.x - col_bad.x);
                            float g = col_bad.y + percent * (col_good.y - col_bad.y);
                            float b = col_bad.z + percent * (col_good.z - col_bad.z);
                            color = ImGui::GetColorU32(ImVec4(r,g,b, alpha));
                        }
                    }

                    if (col > 0)
                        ImGui::SameLine();
                    ImGui::PushID(id);
                    ImGui::PushStyleColor(ImGuiCol_Button, color);
                    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, color);
                    ImGui::PushStyleColor(ImGuiCol_ButtonActive, color); 
                    std::string label = posToMove({row, col});
                    ImGui::Button(label.c_str(), ImVec2(side_length, side_length));
                    ImGui::PopStyleColor(3);
                    ImGui::PopID();
                    
                    if (ImGui::IsItemHovered())
                    {
                        ImGui::BeginTooltip();
                        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
                        ImGui::TextUnformatted(("Move: " + posToMove({row, col})).c_str());
                        ImGui::TextUnformatted(("Fitness: " + std::to_string(score)).c_str());
                        ImGui::PopTextWrapPos();
                        ImGui::EndTooltip();
                    }
                    
                    /*
                    draw_list->AddRectFilled(
                        ImVec2(upper_left_x, upper_left_y),
                        ImVec2(upper_left_x + side_length, upper_left_y + side_length), 
                        color);
                        */

                }
                // ImGui::NextColumn();

                ImGui::NextColumn();
                ImGui::Text("Move ordering");

                for (int row = 0; row < Board::kRows; row++)
                for (int col = 0; col < Board::kCols; col++)
                {
                    int id = row * Board::kCols + col;
                    int place = lastStats.positionOrder[row][col];

                    float a = 1.0f;

                    if ( place == -1 ) a = 0.f;
                    else a = 1.0f - float(place) / (Board::kCols * Board::kRows);

                    ImVec4 col_vec(0.3f, 0.3f, 9.f, a);
                    ImU32 color = ImGui::GetColorU32(col_vec);
                    
                    if (col > 0)
                        ImGui::SameLine();
                    ImGui::PushID(id);
                    ImGui::PushStyleColor(ImGuiCol_Button, color);
                    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, color);
                    ImGui::PushStyleColor(ImGuiCol_ButtonActive, color); 
                    std::string label = "";
                    if ( place != -1 ) label = std::to_string(place);
                    ImGui::Button(label.c_str(), ImVec2(side_length, side_length));
                    ImGui::PopStyleColor(3);
                    ImGui::PopID();
                    
                    if (ImGui::IsItemHovered())
                    {
                        ImGui::BeginTooltip();
                        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
                        ImGui::TextUnformatted("xd");
                        ImGui::PopTextWrapPos();
                        ImGui::EndTooltip();
                    } 



                    /*
                    draw_list->AddRectFilled(
                        ImVec2(upper_left_x, upper_left_y),
                        ImVec2(upper_left_x + side_length, upper_left_y + side_length), 
                        color);
                        */

                }
                
                // ImGui::NextColumn();
                ImGui::Columns(1);
            }

            if (ImGui::CollapsingHeader("Plots")) {
                ImGui::Text("Score function plots");
                int n = agentStats_.size() - 1;

                ImVec2 plotSize(500, 100);

                float * leafs = new float[n];
                float * maxes = new float[n];
                float * mines = new float[n];
                for (int i = 0; i < n; i++) {
                    leafs[i] = agentStats_[i + 1].calculatedLeafs;
                    maxes[i] = agentStats_[i + 1].bestScore;
                    mines[i] = agentStats_[i + 1].worstScore;
                }
                auto[leafsMin, leafsMax] = std::minmax_element(leafs, leafs + n);
                auto[maxesMin, maxesMax] = std::minmax_element(maxes, maxes + n);
                auto[minesMin, minesMax] = std::minmax_element(mines, mines + n);

                ImGui::PlotLines("# Calculated Leafs", leafs, n, 0, NULL, *leafsMin , *leafsMax, plotSize); 
                ImGui::PlotHistogram("Max Fitness", maxes, n, 0, NULL, *maxesMin , *maxesMax, plotSize); 
                ImGui::PlotHistogram("Min Fitness", mines, n, 0, NULL, *minesMin , *minesMax, plotSize); 
                delete [] maxes;
                delete [] mines;
                delete [] leafs;
            }


            ImGui::EndTabItem();
        }




        ImGui::EndTabBar();
    }


    ImGui::End();
}

void Game::renderGameEditor() {
    ImGui::Begin("GameEditor");

    ImGui::Text("History");
    for (int i = 0; i < history_.size(); i++) {
        ImGui::PushID(i);

        sf::Color sfmlColor = painter_.colors[1 + (i%2)];
        ImVec4 color = ImColor(sfmlColor.r, sfmlColor.g, sfmlColor.b);

        if ((i % 5) != 0)
            ImGui::SameLine(0.0f, ImGui::GetStyle().ItemSpacing.y);
        if (ImGui::ColorButton("##history", color) ) {
            board_ = history_[i];

            int toDelete = 0;
            if ( i % 2 == (player_perspective_ - 1) ) 
                toDelete = (history_.size() - i) / 2;
            else 
                toDelete = (history_.size() - i + 1) / 2;

            history_.erase(history_.begin() + i + 1, history_.end());
            agentStats_.erase(agentStats_.end() - toDelete, agentStats_.end());
        }
        ImGui::PopID();
    }

    // Display contents in a scrolling region
    ImGui::End();
}

void Game::drawImgui() {
    ImGui::SFML::Update(window_, clock_.restart());

    // ImGui::ShowDemoWindow();
    renderGameEditor(); 
    renderAgent();

    ImGui::SFML::Render(window_);
}

void Game::playAgent() {
    bool game_on = true;
    while ( game_on && window_.isOpen() ) { 
        std::lock_guard<std::mutex> lock(board_mutex_);
        if (board_.currentPlayer() == agent_.perspective() ) {

            auto agent_move = agent_.getMove(board_); 

            if ( !makeMove(agent_.perspective(), agent_move) ) {
                board_mutex_.unlock();
                return;
            }

            agentStats_.push_back(agent_.getStats());

            if ( board_.isGameOver() )
                game_on = false;
        }
    }
}

void Game::playGame(int player_perspective) {
    player_perspective_ = player_perspective;
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
                cursor_ = { painter_.getLogicalPosition(window_, position) };

                std::lock_guard<std::mutex> lock(board_mutex_);
                if ( board_.currentPlayer() == player_perspective_ ) {
                    if ( board_.possible(cursor_, player_perspective_) ) {
                        Move my_move = posToMove(cursor_);
                        makeMove(player_perspective_, my_move);
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