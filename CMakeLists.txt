cmake_minimum_required(VERSION 3.25)
project(ProgrammingProject_2025S2_101229262 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
        SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG 3.0.2
        GIT_SHALLOW ON
)

FetchContent_MakeAvailable(SFML)

set(SFML_LIBS
        SFML::Audio
        SFML::Graphics
        SFML::Window
        SFML::System
)

include_directories(
    ${CMAKE_SOURCE_DIR}/base/headers
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/game
    ${CMAKE_SOURCE_DIR}/src/dialogue
    ${CMAKE_SOURCE_DIR}/src/engine
    ${CMAKE_SOURCE_DIR}/src/engine/states
)

# Source files
set(GAME_SOURCES
    main.cpp
    src/engine/GameEngine.cpp
    src/engine/DialogueVisitor.cpp
    src/dialogue/Dialogue.cpp
    src/dialogue/DialogueGraph.cpp
    src/dialogue/Choice.cpp
    src/engine/states/MainMenuState.cpp
    src/engine/states/InGameState.cpp
    src/engine/states/LoadGameState.cpp
    src/engine/states/SettingsState.cpp
    src/game/SaveSystem.cpp
        src/engine/DialogueManager.cpp
        src/engine/DialogueManager.cpp
)

add_executable(${PROJECT_NAME} ${GAME_SOURCES})

# Test executable for Stack and Queue
add_executable(test_stack_queue test_stack_queue.cpp)

# Copy assets directory to the build directory
file(COPY ${CMAKE_SOURCE_DIR}/assets DESTINATION ${CMAKE_BINARY_DIR})

# Define asset path constant for code to use - use relative path for portability
add_compile_definitions(ASSETS_PATH="assets/")

target_link_libraries(${PROJECT_NAME} PRIVATE ${SFML_LIBS})
