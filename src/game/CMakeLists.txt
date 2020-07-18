add_library(game Human.hpp Game.hpp Game.cpp Move.hpp Move.cpp Player.hpp)
target_link_libraries(game 
    board agent render
    sfml-graphics sfml-window sfml-system 
    ImGui-SFML::ImGui-SFML
)
target_include_directories(game INTERFACE ..)