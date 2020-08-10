add_library(agent Agent.hpp Agent.cpp Evaluator.hpp Evaluator.cpp Hasher.hpp Hasher.cpp MoveOrganizer.hpp MoveOrganizer.cpp) 
target_link_libraries(agent board render)
target_include_directories(agent INTERFACE ..)