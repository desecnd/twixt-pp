add_library(game Human.hpp Judge.hpp Judge.cpp Move.hpp Move.cpp Player.hpp)
target_link_libraries(game board agent)
target_include_directories(game INTERFACE ..)