cmake_minimum_required(VERSION 3.23)
project (HerbEngine)

set (CMAKE_CXX_STANDARD 11)

add_library (HerbEngine STATIC
    ${PROJECT_SOURCE_DIR}/src/components/physics.cpp
    ${PROJECT_SOURCE_DIR}/src/components/rendering.cpp
    ${PROJECT_SOURCE_DIR}/src/components/sound.cpp
    ${PROJECT_SOURCE_DIR}/src/base.cpp
)

target_link_libraries (HerbEngine LINK_PUBLIC raylib)
target_link_libraries (HerbEngine LINK_PUBLIC glm)
target_link_libraries (HerbEngine LINK_PUBLIC JSON)
target_link_libraries (HerbEngine LINK_PUBLIC Logger)
target_include_directories (HerbEngine PUBLIC ${PROJECT_SOURCE_DIR}/include)

