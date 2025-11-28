# CMake configuration for Dialogue RPG Game
# Minimum CMake version required
cmake_minimum_required(VERSION 3.25)

# Project name and language
project(ProgrammingProject_2025S2_101229262 LANGUAGES CXX)

# Use C++20 standard (required for SFML 3.0 features)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include FetchContent module for downloading dependencies
include(FetchContent)

# Fetch SFML library from GitHub (SFML: Simple and Fast Multimedia Library)
FetchContent_Declare(
        SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG 3.0.2
        GIT_SHALLOW ON
)

# Make SFML available for linking
FetchContent_MakeAvailable(SFML)

# Define SFML modules to link (Audio, Graphics, Window, System)
set(SFML_LIBS
        SFML::Audio
        SFML::Graphics
        SFML::Window
        SFML::System
)

# Add include directories for custom data structures and game code
include_directories(
    ${CMAKE_SOURCE_DIR}/base/headers
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/game
    ${CMAKE_SOURCE_DIR}/src/dialogue
    ${CMAKE_SOURCE_DIR}/src/engine
    ${CMAKE_SOURCE_DIR}/src/engine/states
)

# List all source files for compilation
# Multiple Visitor pattern implementations (RenderVisitor, LogVisitor, DebugVisitor)
set(GAME_SOURCES
    main.cpp
    src/engine/GameEngine.cpp
    src/engine/DialogueRenderVisitor.cpp
    src/engine/DialogueLogVisitor.cpp
    src/engine/DialogueDebugVisitor.cpp
    src/engine/DialogueUI.cpp
    src/dialogue/Dialogue.cpp
    src/dialogue/DialogueGraph.cpp
    src/dialogue/Choice.cpp
    src/engine/states/MainMenuState.cpp
    src/engine/states/InGameState.cpp
    src/engine/states/LoadGameState.cpp
    src/engine/states/SettingsState.cpp
    src/game/SaveSystem.cpp
)

# Create the executable from source files
add_executable(${PROJECT_NAME} ${GAME_SOURCES})

# Copy assets directory to build directory (fonts, music, dialogue scripts)
file(COPY ${CMAKE_SOURCE_DIR}/assets DESTINATION ${CMAKE_BINARY_DIR})

# Define compile-time constant for asset path (relative for portability)
add_compile_definitions(ASSETS_PATH="assets/")

# Link SFML libraries to the executable
target_link_libraries(${PROJECT_NAME} PRIVATE ${SFML_LIBS})
