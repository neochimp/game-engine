file(GLOB_RECURSE ENGINE_SOURCE_FILES CONFIGURE_DEPENDS src/*.cpp)
add_library(CS1950UEngine STATIC ${ENGINE_SOURCE_FILES})

target_include_directories(
        CS1950UEngine PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# glfw and freetype were previously pulled in implicitly; they are explicit now
# because window.cpp includes <GLFW/glfw3.h> and font.cpp includes <freetype/freetype.h>.
target_link_libraries(CS1950UEngine PUBLIC
        ${OPENGL_LIBRARIES}
        glew_static
        glfw
        freetype
        glm
        assimp
        stb
        imgui
        imguizmo
        raudio
)

set(CS1950UEngine_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/include")

# Platform-specific settings
if(WIN32)
    add_compile_definitions(GLEW_STATIC)
    # debug.h includes <windows.h>, whose mmsystem.h defines a PlaySound macro that
    # collides with raudio's PlaySound(Sound). Lean mode skips mmsystem.h; PUBLIC so
    # the games that include Engine headers get it too.
    target_compile_definitions(CS1950UEngine PUBLIC WIN32_LEAN_AND_MEAN NOMINMAX)
    target_link_libraries(CS1950UEngine PUBLIC
            opengl32
            glu32
    )
elseif(UNIX AND NOT APPLE)
    target_link_libraries(CS1950UEngine PUBLIC GL)
endif()
 