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
        SFML::Graphics
        SFML::Audio
)

add_executable(${PROJECT_NAME} main.cpp
        headers/SinglyLinkedNode.h
        headers/DoublyLinkedNode.h
        headers/List.h
        headers/BidirectionalIterator.h
        headers/ForwardIterator.h
        headers/Stack.h
        headers/Queue.h
        headers/HashEntry.h
        headers/Hasher.h
        headers/HashTable.h
        headers/NTree.h
)
target_link_libraries(${PROJECT_NAME} PRIVATE ${SFML_LIBS})
