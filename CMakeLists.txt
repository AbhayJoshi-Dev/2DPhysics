cmake_minimum_required(VERSION 3.25.0)

project(Physics2D)

set(DEPS_DIR ${CMAKE_SOURCE_DIR}/build/external)

add_subdirectory(external/SDL)

file(GLOB GS_SOURCE_FILES src/*.cpp)
file(GLOB GS_HEADER_FILES include/*.h)
  
add_executable(Physics2D ${GS_SOURCE_FILES} ${GS_HEADER_FILES})

target_include_directories(Physics2D PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/
                                            ${CMAKE_CURRENT_SOURCE_DIR}/external/SDL/include/)

target_link_libraries(Physics2D SDL2 SDL2main)

add_custom_command(TARGET Physics2D POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    "${DEPS_DIR}/SDL/Debug/SDL2d.dll"
                    $<TARGET_FILE_DIR:Physics2D>)