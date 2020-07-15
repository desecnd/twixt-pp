add_executable(twixt-game main.cpp)

target_compile_features(twixt-game PRIVATE cxx_std_17)

target_link_libraries(twixt-game
    agent game board 
    sfml-graphics sfml-window sfml-system 
    ImGui-SFML::ImGui-SFML
)
