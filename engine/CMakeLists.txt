# Create the 'Engine' library target
add_library(Engine
        src/XCube2d.cpp
        include/XCube2d.h
        src/AbstractServer.cpp
        include/AbstractServer.h
        include/utils/EngineCommon.h
        include/utils/GameMath.h
        src/EventEngine.cpp
        include/EventEngine.h
        src/GraphicsEngine.cpp
        include/GraphicsEngine.h
        include/NetworkEngine.h
)

# Specify the include directories for the 'Engine' target
target_include_directories(Engine PUBLIC include)

# Link external libraries to the 'Engine' target
target_link_libraries(Engine
        PUBLIC
        $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
        $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
        $<IF:$<TARGET_EXISTS:SDL2_image::SDL2_image>,SDL2_image::SDL2_image,SDL2_image::SDL2_image-static>
)

# Define __DEBUG macro for Debug builds
target_compile_definitions(Engine PUBLIC $<$<CONFIG:Debug>:__DEBUG>)

