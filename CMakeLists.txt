cmake_minimum_required(VERSION 3.20)

if (USING_ANDROID)
    set(VCPKG_TARGET_TRIPLET "arm64-android-dynamic" CACHE STRING "Vcpkg triplet")
#    set(VCPKG_CHAINLOAD_TOOLCHAIN_FILE "C:/Users/charl/.vcpkg-clion/vcpkg/scripts/buildsystems/vcpkg.cmake")
    include("C:/Users/charl/.vcpkg-clion/vcpkg/scripts/buildsystems/vcpkg.cmake")
endif ()

project(Crossplatform)

set(CMAKE_CXX_STANDARD 20)

if(CMAKE_GENERATOR STREQUAL "Xcode")
    add_definitions(-DXCODE_BUILD)
endif()

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zc:__cplusplus /Zc:preprocessor")
endif()

set(INCLUDED_FILES main.cpp)

if (USING_ANDROID)
    add_library(Crossplatform SHARED ${INCLUDED_FILES})
else ()
    add_executable(Crossplatform ${INCLUDED_FILES})
endif ()

# bgfx
set(BGFX_BUILD_EXAMPLES OFF CACHE STRING "" FORCE)
set(BGFX_BUILD_TOOLS ON CACHE STRING "" FORCE)
set(BGFX_BUILD_TOOLS_BIN2C ON CACHE STRING "" FORCE)
set(BGFX_BUILD_TOOLS_SHADER ON CACHE STRING "" FORCE)
set(BGFX_BUILD_TOOLS_GEOMETRY ON CACHE STRING "" FORCE)
set(BGFX_BUILD_TOOLS_TEXTURE ON CACHE STRING "" FORCE)
add_subdirectory("include/bgfx.cmake")

if (USING_ANDROID)
    find_library(NATIVEWINDOW_LIBRARY nativewindow)
    find_library(EGL_LIBRARY EGL)
    find_library(GLESV2_LIBRARY GLESv2)

    target_link_libraries(bgfx PUBLIC ${LOG_LIBRARY}
            ${NATIVEWINDOW_LIBRARY}
            ${EGL_LIBRARY}
            ${GLESV2_LIBRARY}
    )
endif ()

# SDL3
find_package(SDL3 CONFIG REQUIRED)

# glm
find_package(glm CONFIG REQUIRED)

# stb(_image)
find_package(Stb REQUIRED)

target_link_libraries(Crossplatform PRIVATE bx bgfx bimg bimg_decode SDL3::SDL3 glm::glm)
target_include_directories(Crossplatform PRIVATE ${Stb_INCLUDE_DIR} include/bgfx.cmake/bgfx/include/ include/bgfx.cmake/bx/include/ include/bgfx.cmake/bimg/include ${CMAKE_HOME_DIRECTORY}/..assets/shaders)