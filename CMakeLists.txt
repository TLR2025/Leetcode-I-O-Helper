cmake_minimum_required(VERSION 3.5)
project(leetcode-io-helper)
set(CMAKE_CXX_STANDARD 20)
file(GLOB_RECURSE SRC ${PROJECT_SOURCE_DIR}/src/*.cpp)
include_directories(${PROJECT_SOURCE_DIR}/includes)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_executable(lioh ${SRC})

# Copy resources
add_custom_command(
    TARGET lioh POST_BUILD # after the lioh is built, run the command
    COMMAND ${CMAKE_COMMAND} -E copy_directory # CMAKE_COMMAND: the bin file of CMAKE, -E: let the CMAKE use a built-in tool
            ${CMAKE_SOURCE_DIR}/assets
            $<TARGET_FILE_DIR:lioh>/assets
)