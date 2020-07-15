add_library(board Board.hpp Board.cpp Collisions.hpp Direction.hpp Direction.cpp Position.hpp Position.cpp )
target_include_directories(board INTERFACE ..)