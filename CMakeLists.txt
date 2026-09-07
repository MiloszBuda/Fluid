cmake_minimum_required(VERSION 3.20)

project(MyProject C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
        SDL3
        GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
        GIT_TAG        release-3.2.8
        GIT_SHALLOW    TRUE
)

FetchContent_MakeAvailable(SDL3)

add_executable(${PROJECT_NAME}
        src/main.c
)

target_link_libraries(${PROJECT_NAME}
        PRIVATE SDL3::SDL3
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:SDL3::SDL3>
        $<TARGET_FILE_DIR:${PROJECT_NAME}>
        VERBATIM
)