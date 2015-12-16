include_directories(${PROJECT_SOURCE_DIR}/inc)
add_executable(mainApp main.cpp)
target_link_libraries(mainApp common dyninstAPI instructionAPI)
