cmake_minimum_required(VERSION 3.10)

project(HAICL CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include directories
include_directories(include)

# Find libcurl
find_package(CURL REQUIRED)

# nlohmann/json and CLI11 are header-only libraries, so just include the directory

# Source files
file(GLOB_RECURSE SOURCES "src/*.cpp")

# Add executable
add_executable(haicl ${SOURCES})

# Link libraries
target_link_libraries(haicl PRIVATE ${CURL_LIBRARIES})

# Install rules (optional)
install(TARGETS haicl DESTINATION bin)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/README.md DESTINATION share/doc/haicl)


