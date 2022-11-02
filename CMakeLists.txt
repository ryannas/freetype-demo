cmake_minimum_required(VERSION 3.15)

project(opengl-test LANGUAGES CXX C)

file(GLOB source include/*.h src/*.cpp)
file(GLOB glad /home/ryan/Documents/third_parties/glad.c)

find_package(Freetype REQUIRED)

add_executable(run_it ${source} ${glad})

target_include_directories(run_it PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_include_directories(run_it PUBLIC
    ${FREETYPE_INCLUDE_DIRS}
)

target_link_libraries(run_it PUBLIC
    GL
    glfw
    freetype
)
