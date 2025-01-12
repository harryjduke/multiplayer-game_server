# Create the server executable target
add_executable(${SERVER_NAME}
        src/main.cpp
        src/Server.cpp
        src/Server.h)

# Specify the include directories for the server target
target_include_directories(${SERVER_NAME} PRIVATE include)

# Link the 'Engine' library to the server target
target_link_libraries(${SERVER_NAME} Engine)