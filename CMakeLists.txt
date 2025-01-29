cmake_minimum_required(VERSION 3.20)

if (USING_ANDROID)
    set(VCPKG_TARGET_TRIPLET "arm64-android" CACHE STRING "Vcpkg triplet")
    set(VCPKG_CHAINLOAD_TOOLCHAIN_FILE ${CMAKE_TOOLCHAIN_FILE})
    set(CMAKE_TOOLCHAIN_FILE C:/Users/charl/.vcpkg-clion/vcpkg/scripts/buildsystems/vcpkg.cmake)
endif ()

project(Crossplatform)

set(CMAKE_CXX_STANDARD 20)

if(CMAKE_GENERATOR STREQUAL "Xcode")
    add_definitions(-DXCODE_BUILD)
endif()

set(INCLUDED_FILES main.cpp)

if (CMAKE_SYSTEM_NAME STREQUAL "iOS")
    set(INCLUDED_FILES ${INCLUDED_FILES} sdl_bgfx_ios.h sdl_bgfx_ios.mm)
endif ()

if (USING_ANDROID)
    add_library(Crossplatform SHARED ${INCLUDED_FILES})
else ()
    add_executable(Crossplatform ${INCLUDED_FILES})
endif ()

# bgfx
find_package(bgfx CONFIG REQUIRED)

if (USING_ANDROID)
    find_library(NATIVEWINDOW_LIBRARY nativewindow)
    find_library(EGL_LIBRARY EGL)
    find_library(GLESV2_LIBRARY GLESv2)

    target_link_libraries(Crossplatform PRIVATE ${NATIVEWINDOW_LIBRARY} ${EGL_LIBRARY} ${GLESV2_LIBRARY})

# SDL3
add_subdirectory("include/SDL")

# glm
find_package(glm CONFIG REQUIRED)

# stb(_image)
find_package(Stb REQUIRED)

target_link_libraries(Crossplatform PRIVATE bgfx::bx bgfx::bgfx bgfx::bimg bgfx::bimg_decode SDL3::SDL3-shared glm::glm)
target_include_directories(Crossplatform PRIVATE ${Stb_INCLUDE_DIR} SDL3::Headers ${CMAKE_HOME_DIRECTORY}/..assets/shaders)