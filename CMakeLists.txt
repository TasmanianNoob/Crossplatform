cmake_minimum_required(VERSION 3.30)
project(Crossplatform)

set(CMAKE_CXX_STANDARD 20)

add_executable(Crossplatform main.cpp)

# SDL3
find_package(SDL3 CONFIG REQUIRED)

# bgfx
find_package(bgfx CONFIG REQUIRED)

# glm
find_package(glm CONFIG REQUIRED)

# stb(_image)
find_package(Stb REQUIRED)

target_link_libraries(Crossplatform PUBLIC bgfx::bx bgfx::bgfx bgfx::bimg bgfx::bimg_decode SDL3::SDL3 glm::glm)
target_include_directories(Crossplatform PRIVATE ${Stb_INCLUDE_DIR})