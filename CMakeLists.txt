cmake_minimum_required(VERSION 3.15)
project(BrushRoyale VERSION 0.1.0 LANGUAGES CXX C)

# --- 1. Settings ---
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Include FetchContent (The magic tool)
include(FetchContent)

# --- 2. Fetch SFML (Graphics) ---
message(STATUS "Downloading and building SFML... (This may take a minute)")
FetchContent_Declare(
    sfml
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG        3.0.2  # Stable version
)
# We disable audio/network modules of SFML if you don't use them to save build time
set(SFML_BUILD_AUDIO OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_NETWORK OFF CACHE BOOL "" FORCE) # We use ENet for network!
FetchContent_MakeAvailable(sfml)

# --- 3. Fetch ENet (Networking) ---
message(STATUS "Downloading and building ENet...")
FetchContent_Declare(
    enet
    GIT_REPOSITORY https://github.com/lsalzman/enet.git
    GIT_TAG        master
)
FetchContent_GetProperties(enet)
if(NOT enet_POPULATED)
    FetchContent_Populate(enet)
    
    # ENet doesn't have a good CMake file, so we define the library manually here:
    include_directories(${enet_SOURCE_DIR}/include)
    file(GLOB ENET_SOURCES "${enet_SOURCE_DIR}/*.c")
    
    add_library(enet STATIC ${ENET_SOURCES})
    target_include_directories(enet PUBLIC ${enet_SOURCE_DIR}/include)
    
    # ENet on Linux needs to link against the "math" library (m)
    if(UNIX)
        target_link_libraries(enet m)
    endif()
endif()

# --- 4. Common Code ---
add_library(pw_common INTERFACE)
target_include_directories(pw_common INTERFACE 
    ${CMAKE_SOURCE_DIR}/src/common
)
# Common needs access to ENet headers
target_link_libraries(pw_common INTERFACE enet)

# --- 5. Server Executable ---
add_executable(pw_server
    src/server/main.cpp
    src/server/Server.cpp
)
target_link_libraries(pw_server PRIVATE pw_common enet)
target_include_directories(pw_server PRIVATE src/server)

# --- 6. Client Executable ---
add_executable(pw_client
    src/client/main.cpp
    src/client/Client.hpp
)
target_link_libraries(pw_client PRIVATE 
    pw_common 
    enet 
    sfml-graphics 
    sfml-window 
    sfml-system
)
target_include_directories(pw_client PRIVATE src/client)

message(STATUS "Build configured! Binaries will be in: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
