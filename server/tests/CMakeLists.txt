find_package(GTest REQUIRED)

# Only the sources actually exercised by these tests are compiled in.
# Controllers/filters/repositories are deliberately left out: they either
# require a live Drogon event loop + DB connection (integration-test
# territory) or are thin enough that the logic they call into is what's
# actually under test here.
add_executable(maze_race_tests
    MazeGeneratorTest.cc
    PasswordServiceTest.cc
    JwtServiceTest.cc
    GameRoomManagerTest.cc
    RoomServiceTest.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/../services/PasswordService.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/../services/JwtService.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/../services/GameRoomManager.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/../services/RoomService.cc
)

target_include_directories(maze_race_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/..)

target_link_libraries(maze_race_tests PRIVATE
    Drogon::Drogon
    OpenSSL::SSL
    OpenSSL::Crypto
    GTest::gtest
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(maze_race_tests DISCOVERY_MODE PRE_TEST)
