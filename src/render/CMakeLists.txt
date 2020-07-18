add_library(render Painter.hpp Painter.cpp)

target_link_libraries(render 
    board agent 
    sfml-graphics sfml-window sfml-system 
    ImGui-SFML::ImGui-SFML
)

target_link_directories(render INTERFACE ..)