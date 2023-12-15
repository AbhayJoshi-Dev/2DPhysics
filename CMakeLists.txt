cmake_minimum_required(VERSION 3.25.0)

project(2DPhysics)

set(DEPS_DIR ${CMAKE_SOURCE_DIR}/build/external)

add_subdirectory(external/SDL)




set(2DPHYSICS_SOURCE_FILES
src/Body.cpp
src/Manifold.cpp
src/Scene.cpp
src/Shape.cpp
src/Timer.cpp
)

set(2DPHYSICS_HEADER_FILES
include/Body.h
include/Manifold.h
include/Scene.h
include/Shape.h
include/Timer.h
include/Collision.h
include/MathUtils.h
include/DebugDraw.h
)

add_library(2DPhysics ${2DPHYSICS_SOURCE_FILES} ${2DPHYSICS_HEADER_FILES})
target_include_directories(
                         2DPhysics PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include/
                        ${CMAKE_CURRENT_SOURCE_DIR}/external/SDL/include/
)


set(IMGUI_SOURCE_FILES
  external/imgui/imgui.cpp
  external/imgui/imgui_demo.cpp
  external/imgui/imgui_draw.cpp
  external/imgui/imgui_tables.cpp
  external/imgui/imgui_widgets.cpp
  )

set(IMGUI_HEADER_FILES
  external/imgui/imconfig.h
  external/imgui/imgui.h
  external/imgui/imgui_internal.h
  external/imgui/imstb_rectpack.h
  external/imgui/imstb_textedit.h
  external/imgui/imstb_truetype.h
)

add_library(imgui STATIC ${IMGUI_SOURCE_FILES} ${IMGUI_HEADER_FILES})
target_include_directories(imgui PUBLIC external/imgui/)


set(TESTBED_SOURCE_FILES 
testbed/include/Test.h
testbed/src/Test.cpp
testbed/include/Draw.h
testbed/src/Draw.cpp
testbed/src/main.cpp
testbed/include/imgui_impl_sdl2.h
testbed/src/imgui_impl_sdl2.cpp
testbed/include/imgui_impl_sdlrenderer2.h
testbed/src/imgui_impl_sdlrenderer2.cpp
)
  
add_executable(testbed ${TESTBED_SOURCE_FILES})

target_include_directories(testbed PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/testbed/include/
            ${CMAKE_CURRENT_SOURCE_DIR}/include/
            ${CMAKE_CURRENT_SOURCE_DIR}/external/SDL/include/)

target_link_libraries(testbed PUBLIC  SDL2 SDL2main 2DPhysics imgui)

if (MSVC)
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT testbed)
endif()


add_custom_command(
    TARGET testbed POST_BUILD 
    COMMAND ${CMAKE_COMMAND} -E copy_directory
                    "${DEPS_DIR}/SDL/$<CONFIGURATION>"
                    $<TARGET_FILE_DIR:2DPhysics>)