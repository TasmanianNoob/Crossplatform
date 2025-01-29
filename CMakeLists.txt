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

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zc:__cplusplus /Zc:preprocessor")
endif()

set(INCLUDED_FILES main.cpp Base.lproj/LaunchScreen.storyboard)

if (CMAKE_SYSTEM_NAME STREQUAL "iOS")
    set(INCLUDED_FILES ${INCLUDED_FILES} sdl_bgfx_ios.h sdl_bgfx_ios.mm)
endif ()

if (USING_ANDROID)
    add_library(Crossplatform SHARED ${INCLUDED_FILES})
else ()
    add_executable(Crossplatform ${INCLUDED_FILES})
endif ()

if (CMAKE_SYSTEM_NAME STREQUAL "iOS")
    set(input_root "${CMAKE_CURRENT_LIST_DIR}/assets")

    macro(add_resource FILE)
        file(RELATIVE_PATH relpath "${input_root}" "${FILE}")
        get_filename_component(relpath "${relpath}" DIRECTORY)
        target_sources(Crossplatform PRIVATE ${FILE})
        set_property(SOURCE ${FILE} PROPERTY MACOSX_PACKAGE_LOCATION "Resources/${relpath}")
    endmacro()

    file(GLOB_RECURSE ASSET_FILES "${CMAKE_CURRENT_LIST_DIR}/assets/*")
    foreach(ASSET ${ASSET_FILES})
        add_resource(${ASSET})
    endforeach()
endif ()

set_target_properties(Crossplatform PROPERTIES
        MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_LIST_DIR}/Info.plist
        XCODE_ATTRIBUTE_BUNDLE_IDENTIFIER com.tasmanainnoob.crossplatform-ios
        XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER com.tasmanainnoob.crossplatform-ios
)

# bgfx
find_package(bgfx CONFIG REQUIRED)

if (USING_ANDROID)
    find_library(NATIVEWINDOW_LIBRARY nativewindow)
    find_library(EGL_LIBRARY EGL)
    find_library(GLESV2_LIBRARY GLESv2)

    target_link_libraries(Crossplatform PRIVATE ${NATIVEWINDOW_LIBRARY} ${EGL_LIBRARY} ${GLESV2_LIBRARY})
endif ()

# SDL3
if (USING_ANDROID)
    add_subdirectory("include/SDL")
else ()
    find_package(SDL3 CONFIG REQUIRED)
endif ()

# glm
find_package(glm CONFIG REQUIRED)

# stb(_image)
find_package(Stb REQUIRED)

target_link_libraries(Crossplatform PRIVATE bgfx::bx bgfx::bgfx bgfx::bimg bgfx::bimg_decode SDL3::SDL3 glm::glm)
target_include_directories(Crossplatform PRIVATE ${Stb_INCLUDE_DIR} ${CMAKE_HOME_DIRECTORY}/..assets/shaders)

if (USING_ANDROID)
    target_include_directories(Crossplatform PRIVATE SDL3::Headers)
endif ()