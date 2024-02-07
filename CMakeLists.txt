cmake_minimum_required(VERSION 3.22.1)

project(megan VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(THIRD_PARTY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty)
set(SFML_SRC_DIR ${THIRD_PARTY_DIR}/SFML/include)
set(SFML_LIB_DIR ${THIRD_PARTY_DIR}/SFML/lib)
set(SFML_BIN_DIR ${THIRD_PARTY_DIR}/SFML/bin)

include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

set(SOURCES
        src/main.cpp
        src/Bus.cpp
        src/Cpu68000.cpp
        src/Graphics.cpp
        src/VDP.cpp
)

set(HEADERS
        src/Bus.hpp
        src/Cpu68000.hpp
        src/defs.hpp
        src/Graphics.hpp
        src/VDP.hpp
)

add_executable(${CMAKE_PROJECT_NAME} ${SOURCES} ${HEADERS})

target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC
    ${SFML_SRC_DIR}
)

target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE
    ${SFML_LIB_DIR}
)

target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE 
    optimized sfml-graphics
    optimized sfml-system
    optimized sfml-window
    optimized sfml-network
    optimized sfml-audio
)

if(WIN32)
target_link_libraries(${CMAKE_PROJECT_NAME} PUBLIC
    debug sfml-main-d optimized sfml-main
    debug sfml-graphics-d
    debug sfml-system-d
    debug sfml-window-d
    debug sfml-network-d
    debug sfml-audio-d
)

file(COPY ${SFML_BIN_DIR}/ DESTINATION ${PROJECT_BINARY_DIR})
endif() 
