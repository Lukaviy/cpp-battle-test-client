# Test utilities
add_library(test_utils INTERFACE)
target_include_directories(test_utils INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

# All tests in one executable
file(GLOB TEST_SOURCES CONFIGURE_DEPENDS *.cpp)
add_executable(all_tests ${TEST_SOURCES})
target_link_libraries(all_tests PRIVATE sw_battle_lib test_utils Catch2::Catch2WithMain)

# Register tests with CTest
list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
include(CTest)
include(Catch)
catch_discover_tests(all_tests)
