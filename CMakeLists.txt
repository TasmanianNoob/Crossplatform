cmake_minimum_required(VERSION 3.20)

if (USING_ANDROID)
    set(VCPKG_TARGET_TRIPLET "arm64-android-dynamic" CACHE STRING "Vcpkg triplet")
    include("C:/Users/charl/.vcpkg-clion/vcpkg/scripts/buildsystems/vcpkg.cmake")
endif ()

project(Crossplatform)

set(CMAKE_CXX_STANDARD 20)

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zc:__cplusplus /Zc:preprocessor")
endif()

if (USING_ANDROID)
    add_library(Crossplatform SHARED main.cpp)
else ()
    add_executable(Crossplatform main.cpp)
endif ()

# SDL3
find_package(SDL3 CONFIG REQUIRED)

set(LIBRARY_TYPE STATIC)
if (USING_ANDROID)
    set(LIBRARY_TYPE SHARED)
endif ()

# bgfx
#find_package(bgfx CONFIG REQUIRED)
set(BGFX_BUILD_EXAMPLES OFF CACHE STRING "" FORCE)
set(BGFX_LIBRARY_TYPE ${LIBRARY_TYPE} CACHE STRING "" FORCE)
set(BX_LIBRARY_TYPE ${LIBRARY_TYPE} CACHE STRING "" FORCE)
set(BIMG_LIBRARY_TYPE ${LIBRARY_TYPE} CACHE STRING "" FORCE)
add_subdirectory("include/bgfx.cmake")

# glm
find_package(glm CONFIG REQUIRED)

# stb(_image)
find_package(Stb REQUIRED)

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

target_link_libraries(Crossplatform PRIVATE bx bgfx bimg bimg_decode SDL3::SDL3 glm::glm)
target_include_directories(Crossplatform PRIVATE ${Stb_INCLUDE_DIR} include/bgfx.cmake/bgfx/include/ include/bgfx.cmake/bx/include/ include/bgfx.cmake/bimg/include)