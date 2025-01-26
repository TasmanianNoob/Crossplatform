cmake_minimum_required(VERSION 3.20)
project(Crossplatform)

set(VCPKG_TARGET_TRIPLET "arm64-android" CACHE STRING "Vcpkg triplet")
include("C:/Users/charl/.vcpkg-clion/vcpkg/scripts/buildsystems/vcpkg.cmake")
#message("VCPKG @: ${CMAKE_TOOLCHAIN_FILE}")
#message("VCPKG @: $ENV{ANDROID_NDK_HOME}")
#message("VCPKG @: $ENV{ANDROID_NDK_ROOT}")
#set(CMAKE_TOOLCHAIN_FILE "${CMAKE_TOOLCHAIN_FILE}" CACHE STRING "")

set(CMAKE_CXX_STANDARD 20)

#add_executable(Crossplatform main.cpp)
add_library(Crossplatform SHARED main.cpp)

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