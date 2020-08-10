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

// Helper to display a little (?) mark which shows a tooltip when hovered.
// In your own code you may want to display an actual icon if you are using a merged icon fonts (see docs/FONTS.md)
static void HelpMarker(const char* desc)
{
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(desc);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}

// Helper to display basic user controls.
void ImGui::ShowUserGuide()
{
    ImGuiIO& io = ImGui::GetIO();
    ImGui::BulletText("Double-click on title bar to collapse window.");
    ImGui::BulletText(
        "Click and drag on lower corner to resize window\n"
        "(double-click to auto fit window to its contents).");
    ImGui::BulletText("CTRL+Click on a slider or drag box to input value as text.");
    ImGui::BulletText("TAB/SHIFT+TAB to cycle through keyboard editable fields.");
    if (io.FontAllowUserScaling)
        ImGui::BulletText("CTRL+Mouse Wheel to zoom window contents.");
    ImGui::BulletText("While inputing text:\n");
    ImGui::Indent();
    ImGui::BulletText("CTRL+Left/Right to word jump.");
    ImGui::BulletText("CTRL+A or double-click to select all.");
    ImGui::BulletText("CTRL+X/C/V to use clipboard cut/copy/paste.");
    ImGui::BulletText("CTRL+Z,CTRL+Y to undo/redo.");
    ImGui::BulletText("ESCAPE to revert.");
    ImGui::BulletText("You can apply arithmetic operators +,*,/ on numerical values.\nUse +- to subtract.");
    ImGui::Unindent();
    ImGui::BulletText("With keyboard navigation enabled:");
    ImGui::Indent();
    ImGui::BulletText("Arrow keys to navigate.");
    ImGui::BulletText("Space to activate a widget.");
    ImGui::BulletText("Return to input text into a widget.");
    ImGui::BulletText("Escape to deactivate a widget, close popup, exit child window.");
    ImGui::BulletText("Alt to jump to the menu layer of a window.");
    ImGui::BulletText("CTRL+Tab to select a window.");
    ImGui::Unindent();
}

// [SECTION] Example App: Custom Rendering using ImDrawList API / ShowExampleAppCustomRendering()
//-----------------------------------------------------------------------------

// Demonstrate using the low-level ImDrawList to draw custom shapes.
void Game::renderAgent()
{
    ImGui::Begin("Agent Debug");

    // Tip: If you do a lot of custom rendering, you probably want to use your own geometrical types and benefit of
    // overloaded operators, etc. Define IM_VEC2_CLASS_EXTRA in imconfig.h to create implicit conversions between your
    // types and ImVec2/ImVec4. Dear ImGui defines overloaded operators but they are internal to imgui.cpp and not
    // exposed outside (to avoid messing with your types) In this example we are not using the maths operators!
    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    if (ImGui::BeginTabBar("##TabBar"))
    {
        if (ImGui::BeginTabItem("Primitives"))
        {
            ImGui::PushItemWidth(-ImGui::GetFontSize() * 10);

            // Draw gradients
            // (note that those are currently exacerbating our sRGB/Linear issues)
            ImGui::Text("Gradients");
            ImVec2 gradient_size = ImVec2(ImGui::CalcItemWidth(), ImGui::GetFrameHeight());
            {
                ImVec2 p0 = ImGui::GetCursorScreenPos();
                ImVec2 p1 = ImVec2(p0.x + gradient_size.x, p0.y + gradient_size.y);
                ImU32 col_a = ImGui::GetColorU32(ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
                ImU32 col_b = ImGui::GetColorU32(ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
                draw_list->AddRectFilledMultiColor(p0, p1, col_a, col_b, col_b, col_a);
                ImGui::InvisibleButton("##gradient1", gradient_size);
            }
            {
                ImVec2 p0 = ImGui::GetCursorScreenPos();
                ImVec2 p1 = ImVec2(p0.x + gradient_size.x, p0.y + gradient_size.y);
                ImU32 col_a = ImGui::GetColorU32(ImVec4(0.0f, 1.0f, 0.0f, 1.0f));
                ImU32 col_b = ImGui::GetColorU32(ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
                draw_list->AddRectFilledMultiColor(p0, p1, col_a, col_b, col_b, col_a);
                ImGui::InvisibleButton("##gradient2", gradient_size);
            }

            // Draw a bunch of primitives
            ImGui::Text("All primitives");
            static float sz = 36.0f;
            static float thickness = 3.0f;
            static int ngon_sides = 6;
            static bool circle_segments_override = false;
            static int circle_segments_override_v = 12;
            static ImVec4 colf = ImVec4(1.0f, 1.0f, 0.4f, 1.0f);
            ImGui::DragFloat("Size", &sz, 0.2f, 2.0f, 72.0f, "%.0f");
            ImGui::DragFloat("Thickness", &thickness, 0.05f, 1.0f, 8.0f, "%.02f");
            ImGui::SliderInt("N-gon sides", &ngon_sides, 3, 12);
            ImGui::Checkbox("##circlesegmentoverride", &circle_segments_override);
            ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
            if (ImGui::SliderInt("Circle segments", &circle_segments_override_v, 3, 40))
                circle_segments_override = true;
            ImGui::ColorEdit4("Color", &colf.x);
            const ImVec2 p = ImGui::GetCursorScreenPos();
            const ImU32 col = ImColor(colf);
            const float spacing = 10.0f;
            const ImDrawCornerFlags corners_none = 0;
            const ImDrawCornerFlags corners_all = ImDrawCornerFlags_All;
            const ImDrawCornerFlags corners_tl_br = ImDrawCornerFlags_TopLeft | ImDrawCornerFlags_BotRight;
            const int circle_segments = circle_segments_override ? circle_segments_override_v : 0;
            float x = p.x + 4.0f, y = p.y + 4.0f;
            for (int n = 0; n < 2; n++)
            {
                // First line uses a thickness of 1.0f, second line uses the configurable thickness
                float th = (n == 0) ? 1.0f : thickness;
                draw_list->AddNgon(ImVec2(x + sz*0.5f, y + sz*0.5f), sz*0.5f, col, ngon_sides, th);         x += sz + spacing;  // N-gon
                draw_list->AddCircle(ImVec2(x + sz*0.5f, y + sz*0.5f), sz*0.5f, col, circle_segments, th);  x += sz + spacing;  // Circle
                draw_list->AddRect(ImVec2(x, y), ImVec2(x + sz, y + sz), col, 0.0f,  corners_none, th);     x += sz + spacing;  // Square
                draw_list->AddRect(ImVec2(x, y), ImVec2(x + sz, y + sz), col, 10.0f, corners_all, th);      x += sz + spacing;  // Square with all rounded corners
                draw_list->AddRect(ImVec2(x, y), ImVec2(x + sz, y + sz), col, 10.0f, corners_tl_br, th);    x += sz + spacing;  // Square with two rounded corners
                draw_list->AddTriangle(ImVec2(x+sz*0.5f,y), ImVec2(x+sz, y+sz-0.5f), ImVec2(x, y+sz-0.5f), col, th);      x += sz + spacing;      // Triangle
                draw_list->AddTriangle(ImVec2(x+sz*0.2f,y), ImVec2(x, y+sz-0.5f), ImVec2(x+sz*0.4f, y+sz-0.5f), col, th); x += sz*0.4f + spacing; // Thin triangle
                draw_list->AddLine(ImVec2(x, y), ImVec2(x + sz, y), col, th);                               x += sz + spacing;  // Horizontal line (note: drawing a filled rectangle will be faster!)
                draw_list->AddLine(ImVec2(x, y), ImVec2(x, y + sz), col, th);                               x += spacing;       // Vertical line (note: drawing a filled rectangle will be faster!)
                draw_list->AddLine(ImVec2(x, y), ImVec2(x + sz, y + sz), col, th);                          x += sz + spacing;  // Diagonal line
                draw_list->AddBezierCurve(ImVec2(x, y), ImVec2(x + sz*1.3f, y + sz*0.3f), ImVec2(x + sz - sz*1.3f, y + sz - sz*0.3f), ImVec2(x + sz, y + sz), col, th);
                x = p.x + 4;
                y += sz + spacing;
            }
            draw_list->AddNgonFilled(ImVec2(x + sz * 0.5f, y + sz * 0.5f), sz*0.5f, col, ngon_sides);   x += sz + spacing;  // N-gon
            draw_list->AddCircleFilled(ImVec2(x + sz*0.5f, y + sz*0.5f), sz*0.5f, col, circle_segments);x += sz + spacing;  // Circle
            draw_list->AddRectFilled(ImVec2(x, y), ImVec2(x + sz, y + sz), col);                        x += sz + spacing;  // Square
            draw_list->AddRectFilled(ImVec2(x, y), ImVec2(x + sz, y + sz), col, 10.0f);                 x += sz + spacing;  // Square with all rounded corners
            draw_list->AddRectFilled(ImVec2(x, y), ImVec2(x + sz, y + sz), col, 10.0f, corners_tl_br);  x += sz + spacing;  // Square with two rounded corners
            draw_list->AddTriangleFilled(ImVec2(x+sz*0.5f,y), ImVec2(x+sz, y+sz-0.5f), ImVec2(x, y+sz-0.5f), col);      x += sz + spacing;      // Triangle
            draw_list->AddTriangleFilled(ImVec2(x+sz*0.2f,y), ImVec2(x, y+sz-0.5f), ImVec2(x+sz*0.4f, y+sz-0.5f), col); x += sz*0.4f + spacing; // Thin triangle
            draw_list->AddRectFilled(ImVec2(x, y), ImVec2(x + sz, y + thickness), col);                 x += sz + spacing;  // Horizontal line (faster than AddLine, but only handle integer thickness)
            draw_list->AddRectFilled(ImVec2(x, y), ImVec2(x + thickness, y + sz), col);                 x += spacing*2.0f;  // Vertical line (faster than AddLine, but only handle integer thickness)
            draw_list->AddRectFilled(ImVec2(x, y), ImVec2(x + 1, y + 1), col);                          x += sz;            // Pixel (faster than AddLine)
            draw_list->AddRectFilledMultiColor(ImVec2(x, y), ImVec2(x + sz, y + sz), IM_COL32(0, 0, 0, 255), IM_COL32(255, 0, 0, 255), IM_COL32(255, 255, 0, 255), IM_COL32(0, 255, 0, 255));
            ImGui::Dummy(ImVec2((sz + spacing) * 9.8f, (sz + spacing) * 3));

            ImGui::PopItemWidth();
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Canvas"))
        {
            static ImVector<ImVec2> points;
            static bool adding_line = false;
            if (ImGui::Button("Clear")) points.clear();
            if (points.Size >= 2) { ImGui::SameLine(); if (ImGui::Button("Undo")) { points.pop_back(); points.pop_back(); } }
            ImGui::Text("Left-click and drag to add lines,\nRight-click to undo");

            // Here we are using InvisibleButton() as a convenience to 1) advance the cursor and 2) allows us to use
            // IsItemHovered(). But you can also draw directly and poll mouse/keyboard by yourself.
            // You can manipulate the cursor using GetCursorPos() and SetCursorPos().
            // If you only use the ImDrawList API, you can notify the owner window of its extends with SetCursorPos(max).
            ImVec2 canvas_p = ImGui::GetCursorScreenPos();       // ImDrawList API uses screen coordinates!
            ImVec2 canvas_sz = ImGui::GetContentRegionAvail();   // Resize canvas to what's available
            if (canvas_sz.x < 50.0f) canvas_sz.x = 50.0f;
            if (canvas_sz.y < 50.0f) canvas_sz.y = 50.0f;
            draw_list->AddRectFilledMultiColor(canvas_p, ImVec2(canvas_p.x + canvas_sz.x, canvas_p.y + canvas_sz.y), IM_COL32(50, 50, 50, 255), IM_COL32(50, 50, 60, 255), IM_COL32(60, 60, 70, 255), IM_COL32(50, 50, 60, 255));
            draw_list->AddRect(canvas_p, ImVec2(canvas_p.x + canvas_sz.x, canvas_p.y + canvas_sz.y), IM_COL32(255, 255, 255, 255));

            bool adding_preview = false;
            ImGui::InvisibleButton("canvas", canvas_sz);
            ImVec2 mouse_pos_global = ImGui::GetIO().MousePos;
            ImVec2 mouse_pos_canvas = ImVec2(mouse_pos_global.x - canvas_p.x, mouse_pos_global.y - canvas_p.y);
            if (adding_line)
            {
                adding_preview = true;
                points.push_back(mouse_pos_canvas);
                if (!ImGui::IsMouseDown(0))
                    adding_line = adding_preview = false;
            }
            if (ImGui::IsItemHovered())
            {
                if (!adding_line && ImGui::IsMouseClicked(0))
                {
                    points.push_back(mouse_pos_canvas);
                    adding_line = true;
                }
                if (ImGui::IsMouseClicked(1) && !points.empty())
                {
                    adding_line = adding_preview = false;
                    points.pop_back();
                    points.pop_back();
                }
            }

            // Draw all lines in the canvas (with a clipping rectangle so they don't stray out of it).
            draw_list->PushClipRect(canvas_p, ImVec2(canvas_p.x + canvas_sz.x, canvas_p.y + canvas_sz.y), true);
            for (int i = 0; i < points.Size - 1; i += 2)
                draw_list->AddLine(ImVec2(canvas_p.x + points[i].x, canvas_p.y + points[i].y), ImVec2(canvas_p.x + points[i + 1].x, canvas_p.y + points[i + 1].y), IM_COL32(255, 255, 0, 255), 2.0f);
            draw_list->PopClipRect();
            if (adding_preview)
                points.pop_back();
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("BG/FG draw lists"))
        {
            static bool draw_bg = true;
            static bool draw_fg = true;
            ImGui::Checkbox("Draw in Background draw list", &draw_bg);
            ImGui::SameLine(); HelpMarker("The Background draw list will be rendered below every Dear ImGui windows.");
            ImGui::Checkbox("Draw in Foreground draw list", &draw_fg);
            ImGui::SameLine(); HelpMarker("The Foreground draw list will be rendered over every Dear ImGui windows.");
            ImVec2 window_pos = ImGui::GetWindowPos();
            ImVec2 window_size = ImGui::GetWindowSize();
            ImVec2 window_center = ImVec2(window_pos.x + window_size.x * 0.5f, window_pos.y + window_size.y * 0.5f);
            if (draw_bg)
                ImGui::GetBackgroundDrawList()->AddCircle(window_center, window_size.x * 0.6f, IM_COL32(255, 0, 0, 200), 0, 10 + 4);
            if (draw_fg)
                ImGui::GetForegroundDrawList()->AddCircle(window_center, window_size.y * 0.6f, IM_COL32(0, 255, 0, 200), 0, 10);
            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }

    ImGui::End();
}

void Game::drawImgui() {
    ImGui::SFML::Update(window_, clock_.restart());

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
            history_.erase(history_.begin() + i + 1, history_.end());
        }
        ImGui::PopID();
    }

    // Display contents in a scrolling region
    ImGui::End();

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

            agentStats = agent_.getStats();

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

                std::lock_guard<std::mutex> lock(board_mutex_);
                if ( board_.currentPlayer() == player_perspective_ ) {
                    Position pos { painter_.getLogicalPosition(window_, position) };
                    if ( board_.possible(pos, player_perspective_) ) {
                        Move my_move = posToMove(pos);
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