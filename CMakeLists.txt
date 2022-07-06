cmake_minimum_required(VERSION 3.23)
project (Engine)

set (CMAKE_CXX_STANDARD 11)

add_library (Engine STATIC
    ${PROJECT_SOURCE_DIR}/src/base.cpp
    ${PROJECT_SOURCE_DIR}/src/components/physics.cpp
    ${PROJECT_SOURCE_DIR}/src/components/rendering.cpp
    ${PROJECT_SOURCE_DIR}/src/components/sound.cpp
)

target_link_libraries (Engine LINK_PUBLIC sfml-system sfml-window sfml-graphics sfml-network sfml-audio)
target_link_libraries (Engine LINK_PUBLIC JSON)
target_link_libraries (Engine LINK_PUBLIC Logger)
target_include_directories (Engine PUBLIC ${PROJECT_SOURCE_DIR}/include)
