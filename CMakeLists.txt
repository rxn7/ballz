cmake_minimum_required(VERSION 3.28)
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)

project(ballz VERSION 0.0 LANGUAGES C)

file(GLOB_RECURSE sources CONFIGURE_DEPENDS src/*.c)
add_executable(ballz ${sources})

# Link sdl3
add_subdirectory(vendor/sdl EXCLUDE_FROM_ALL)
target_link_libraries(ballz PRIVATE SDL3::SDL3 m)

set(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -Wpedantic")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")

if(WIN32)
    add_custom_command(
        TARGET ballz POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E copy $<TARGET_FILE:SDL3::SDL3-shared> $<TARGET_FILE_DIR:ballz>
        VERBATIM
    )
endif()

# add_custom_target(copy_assets
#   COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_CURRENT_BINARY_DIR}/assets
#   COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/assets ${CMAKE_CURRENT_BINARY_DIR}/assets
# )
# add_dependencies(ballz copy_assets)
