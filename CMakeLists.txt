cmake_minimum_required(VERSION 3.10)
project(MySFMLProject)

set (CMAKE_CXX_STANDARD 17)

message(STATUS "C Compiler: ${CMAKE_C_COMPILER}")
message(STATUS "C++ Compiler: ${CMAKE_CXX_COMPILER}")

# Flags
if (CMAKE_CXX_COMPILER_ID MATCHES "MSVC") # MSVC Flag
    set(CMAKE_CXX_FLAGS_RELEASE "/O2")
    set(CMAKE_C_FLAGS_RELEASE "/O2")
elseif (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang") #GCC/Clang Flag
    set(CMAKE_CXX_FLAGS_RELEASE "-O2")
    set(CMAKE_C_FLAGS_RELEASE "-O2")
endif()

# SFML directory
set(SFML_DIR "C:/Libraries/SFML/SFML-2.6.2/lib/cmake/SFML")

# Find SFML package
find_package(SFML 2.6.2 REQUIRED COMPONENTS graphics window system)

# Gather all .cpp files in the directory
file(GLOB SOURCES "src/*.cpp")

# For each test file, create a target
foreach(SOURCE_FILE ${SOURCES})
    # Extract the base name (without extension) to use as the target name
    get_filename_component(TARGET_NAME ${SOURCE_FILE} NAME_WE)
    
    # Create executable for each source file
    add_executable(${TARGET_NAME} ${SOURCE_FILE})
    
    # Link SFML libraries to each executable
    target_link_libraries(${TARGET_NAME} sfml-graphics sfml-window sfml-system sfml-network)
endforeach()