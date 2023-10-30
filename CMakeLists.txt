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
)

add_library(2DPhysics ${2DPHYSICS_SOURCE_FILES} ${2DPHYSICS_HEADER_FILES})
target_include_directories(
                         2DPhysics PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include/
                        ${CMAKE_CURRENT_SOURCE_DIR}/external/SDL/include/
)






set(TESTBED_SOURCE_FILES 
testbed/include/Core.h
testbed/src/Core.cpp
testbed/include/Debug_Draw.h
testbed/src/Debug_Draw.cpp
testbed/src/main.cpp
)
  
add_executable(testbed ${TESTBED_SOURCE_FILES})

target_include_directories(testbed PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/testbed/include/
                                            ${CMAKE_CURRENT_SOURCE_DIR}/external/SDL/include/
                                            ${CMAKE_CURRENT_SOURCE_DIR}/include/)

target_link_libraries(testbed PUBLIC  SDL2 SDL2main 2DPhysics)

if (MSVC)
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT testbed)
endif()


add_custom_command(
    TARGET testbed POST_BUILD 
    COMMAND ${CMAKE_COMMAND} -E copy_directory
                    "${DEPS_DIR}/SDL/$<CONFIGURATION>"
                    $<TARGET_FILE_DIR:2DPhysics>)