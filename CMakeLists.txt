# --- SETUP ---
cmake_minimum_required(VERSION 3.21)

# Build with shared libraries by default
option(BUILD_SHARED_LIBS "Build using shared libraries" ON)

# Setup vcpkg
if (NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    if (DEFINED ENV{VCPKG_ROOT})
        cmake_path(SET CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
        message(STATUS "Using toolchain file from VCPKG_ROOT: ${CMAKE_TOOLCHAIN_FILE}")
    endif ()
else ()
    message(STATUS "Using provided CMAKE_TOOLCHAIN_FILE: ${CMAKE_TOOLCHAIN_FILE}")
endif ()

# Setup project
project(multiplayer-game_server)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)

# Setup server
set(SERVER_NAME "server")

# Setup build output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${${CMAKE_PROJECT_NAME}_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${${CMAKE_PROJECT_NAME}_BINARY_DIR}/bin")

# --- DEPENDENCIES ---
if (EXISTS "${CMAKE_SOURCE_DIR}/libs")
    set(CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/libs")
    message(STATUS "Local /libs folder will be checked for dependencies.")
endif ()

find_package(SDL2 CONFIG REQUIRED)
find_package(SDL2_image CONFIG REQUIRED)
find_package(GTest CONFIG REQUIRED)
find_package(msgpack-cxx CONFIG REQUIRED)


# --- SUBDIRECTORIES ---
add_subdirectory(engine)
add_subdirectory(server)

# Set startup project (for visual studio solution generation)  ---
if (CMAKE_GENERATOR MATCHES "Visual Studio")
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${SERVER_NAME})
endif ()
