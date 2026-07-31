cmake_minimum_required(VERSION 3.16)
project(maze_race_server CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Drogon CONFIG REQUIRED)
find_package(OpenSSL REQUIRED)

file(GLOB_RECURSE SRC_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/controllers/*.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/filters/*.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/services/*.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/repositories/*.cc
)

add_executable(${PROJECT_NAME} main.cc ${SRC_FILES})

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_definitions(${PROJECT_NAME} PRIVATE SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}")

target_link_libraries(${PROJECT_NAME} PRIVATE Drogon::Drogon OpenSSL::SSL OpenSSL::Crypto)
