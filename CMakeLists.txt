# Latest version of CMake
# Declaring Language of compiler used and project name
cmake_minimum_required(VERSION 3.16)
project(MultiThreadedWebServer LANGUAGES CXX)

# Standards for portable
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Add executable using these 3 as source
# Reason that header is not needed is the include and the target include dir declared on line 17
add_executable(webserver
    "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Connection.cpp"
)

# Add target dir for headers
target_include_directories(webserver PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)
