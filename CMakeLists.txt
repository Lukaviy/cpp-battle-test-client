cmake_minimum_required(VERSION 3.27)
project(sw_battle_test)

set(CMAKE_CXX_STANDARD 20)

# Option to enable/disable tests
# To enable tests, configure with: cmake -DBUILD_TESTS=ON ..
option(BUILD_TESTS "Build the tests" OFF)

# Main executable
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS src/*.cpp src/*.hpp)
list(FILTER SOURCES EXCLUDE REGEX "src/main.cpp")
add_library(sw_battle_lib ${SOURCES})
target_include_directories(sw_battle_lib PUBLIC src/)

add_executable(sw_battle_test src/main.cpp)
target_link_libraries(sw_battle_test sw_battle_lib)

set_target_properties(sw_battle_test PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
        RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/Debug"
        RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/Release"
)

set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT sw_battle_test)

# Tests (only if BUILD_TESTS is enabled)
if(BUILD_TESTS)
    # Fetch Catch2
    include(FetchContent)
    FetchContent_Declare(
      Catch2
      GIT_REPOSITORY https://github.com/catchorg/Catch2.git
      GIT_TAG        v3.5.2
    )
    FetchContent_MakeAvailable(Catch2)

    enable_testing()

    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests/CMakeLists.txt")
        add_subdirectory(tests)
    endif()
endif()
