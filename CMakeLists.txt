cmake_minimum_required(VERSION 3.16)
project(CS1950UStencil LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# CMake 4.x rejects dependencies that declare cmake_minimum_required < 3.5
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)

# file.h uses PROJECT_ROOT to locate "resources/..." at runtime
add_compile_definitions(PROJECT_ROOT="${CMAKE_CURRENT_SOURCE_DIR}")

# ------------------------------------------------------------------------------
# Dependency versions (downloaded into <build>/_deps at configure time; needs git).
# To share one download across build folders: -DFETCHCONTENT_BASE_DIR=C:/dev/deps
# ------------------------------------------------------------------------------
set(ASSIMP_REF   "v6.0.2")
set(FREETYPE_REF "VER-2-14-2")
set(GLEW_REF     "glew-cmake-2.2.0")
set(GLFW_REF     "3.4")
set(GLM_REF      "1.0.3")
set(IMGUI_REF    "v1.92.9")
# No releases exist for these; pin a 40-character commit SHA instead of "master".
set(IMGUIZMO_REF "master")
set(RAUDIO_REF   "master")
set(STB_REF      "master")

include(FetchContent)
set(FETCHCONTENT_QUIET OFF)
set(FETCHCONTENT_UPDATES_DISCONNECTED ON)
set(CMAKE_SKIP_INSTALL_RULES ON)

# declare_dep(<name> <url> <tag-or-sha> [NO_CMAKE])  NO_CMAKE = download only
function(declare_dep name url ref)
    cmake_parse_arguments(ARG "NO_CMAKE" "" "" ${ARGN})
    set(shallow ON)
    string(LENGTH "${ref}" ref_len)
    if(ref_len EQUAL 40 AND ref MATCHES "^[0-9a-fA-F]+$")
        set(shallow OFF)  # shallow clones can't fetch a bare commit SHA
    endif()
    set(extra "")
    if(ARG_NO_CMAKE)
        set(extra SOURCE_SUBDIR __no_cmake__)
    endif()
    FetchContent_Declare(${name}
            GIT_REPOSITORY ${url}
            GIT_TAG        ${ref}
            GIT_SHALLOW    ${shallow}
            GIT_PROGRESS   ON
            ${extra})
endfunction()

find_package(OpenGL REQUIRED)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

# --- GLFW ---------------------------------------------------------------------
set(GLFW_BUILD_DOCS     OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS    OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL        OFF CACHE BOOL "" FORCE)
declare_dep(glfw https://github.com/glfw/glfw.git ${GLFW_REF})
FetchContent_MakeAvailable(glfw)

# --- GLEW (Engine links `glew_static`) -----------------------------------------
set(ONLY_LIBS ON CACHE BOOL "" FORCE)
declare_dep(glew https://github.com/Perlmint/glew-cmake.git ${GLEW_REF})
FetchContent_MakeAvailable(glew)
target_include_directories(libglew_static SYSTEM PUBLIC $<BUILD_INTERFACE:${glew_SOURCE_DIR}/include>)
target_compile_definitions(libglew_static PUBLIC GLEW_STATIC)
set_target_properties(libglew_shared PROPERTIES EXCLUDE_FROM_ALL TRUE)
add_library(glew_static ALIAS libglew_static)

# --- GLM (header-only) ----------------------------------------------------------
declare_dep(glm https://github.com/g-truc/glm.git ${GLM_REF} NO_CMAKE)
FetchContent_MakeAvailable(glm)
add_library(glm INTERFACE)
target_include_directories(glm SYSTEM INTERFACE ${glm_SOURCE_DIR})
target_compile_definitions(glm INTERFACE GLM_ENABLE_EXPERIMENTAL)  # lets glm/gtx/* headers compile

# --- FreeType (optional codecs off => no extra system deps) ----------------------
set(FT_DISABLE_ZLIB     ON CACHE BOOL "" FORCE)
set(FT_DISABLE_BZIP2    ON CACHE BOOL "" FORCE)
set(FT_DISABLE_PNG      ON CACHE BOOL "" FORCE)
set(FT_DISABLE_HARFBUZZ ON CACHE BOOL "" FORCE)
set(FT_DISABLE_BROTLI   ON CACHE BOOL "" FORCE)
declare_dep(freetype https://github.com/freetype/freetype.git ${FREETYPE_REF})
FetchContent_MakeAvailable(freetype)

# --- Assimp ---------------------------------------------------------------------
# Students can load any model format, so every importer is built by default. This is
# the slowest dependency to compile (once per build folder). For a faster first build,
# configure with -DENGINE_ASSIMP_ALL_IMPORTERS=OFF to build only the common formats below.
option(ENGINE_ASSIMP_ALL_IMPORTERS "Build every Assimp importer (slower first build)" ON)
set(ASSIMP_BUILD_TESTS        OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
set(ASSIMP_INSTALL            OFF CACHE BOOL "" FORCE)
set(ASSIMP_NO_EXPORT          ON  CACHE BOOL "" FORCE)
set(ASSIMP_WARNINGS_AS_ERRORS OFF CACHE BOOL "" FORCE)
if(NOT ENGINE_ASSIMP_ALL_IMPORTERS)
    set(ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT OFF CACHE BOOL "" FORCE)
    foreach(fmt OBJ FBX GLTF COLLADA 3DS PLY STL BLEND X MD5)   # glTF covers .gltf and .glb
        set(ASSIMP_BUILD_${fmt}_IMPORTER ON CACHE BOOL "" FORCE)
    endforeach()
endif()
declare_dep(assimp https://github.com/assimp/assimp.git ${ASSIMP_REF})
FetchContent_MakeAvailable(assimp)

# --- Dear ImGui (ui.cpp includes <backends/...>, so both include dirs) ----------
declare_dep(imgui https://github.com/ocornut/imgui.git ${IMGUI_REF} NO_CMAKE)
FetchContent_MakeAvailable(imgui)
add_library(imgui STATIC
        ${imgui_SOURCE_DIR}/imgui.cpp
        ${imgui_SOURCE_DIR}/imgui_demo.cpp
        ${imgui_SOURCE_DIR}/imgui_draw.cpp
        ${imgui_SOURCE_DIR}/imgui_tables.cpp
        ${imgui_SOURCE_DIR}/imgui_widgets.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
        ${imgui_SOURCE_DIR}/misc/cpp/imgui_stdlib.cpp)   # InputText with std::string
target_include_directories(imgui SYSTEM PUBLIC
        ${imgui_SOURCE_DIR} ${imgui_SOURCE_DIR}/backends ${imgui_SOURCE_DIR}/misc/cpp)
target_link_libraries(imgui PUBLIC glfw OpenGL::GL)

# --- ImGuizmo (ImGuizmo.cpp sits at the repo root or in src/) -------------------
declare_dep(imguizmo https://github.com/CedricGuillemet/ImGuizmo.git ${IMGUIZMO_REF} NO_CMAKE)
FetchContent_MakeAvailable(imguizmo)
file(GLOB_RECURSE IMGUIZMO_SOURCES ${imguizmo_SOURCE_DIR}/*ImGuizmo.cpp)
add_library(imguizmo STATIC ${IMGUIZMO_SOURCES})
target_include_directories(imguizmo SYSTEM PUBLIC ${imguizmo_SOURCE_DIR} ${imguizmo_SOURCE_DIR}/src)
target_link_libraries(imguizmo PUBLIC imgui)

# --- raudio (standalone; miniaudio is bundled) ------------------------------------
declare_dep(raudio https://github.com/raysan5/raudio.git ${RAUDIO_REF} NO_CMAKE)
FetchContent_MakeAvailable(raudio)
add_library(raudio STATIC ${raudio_SOURCE_DIR}/src/raudio.c)
target_include_directories(raudio SYSTEM PUBLIC ${raudio_SOURCE_DIR}/src)
target_compile_definitions(raudio PUBLIC RAUDIO_STANDALONE)
if(WIN32)
    target_link_libraries(raudio PUBLIC winmm)
elseif(APPLE)
    target_link_libraries(raudio PUBLIC
            "-framework CoreFoundation" "-framework CoreAudio" "-framework AudioToolbox")
else()
    find_package(Threads REQUIRED)
    target_link_libraries(raudio PUBLIC Threads::Threads ${CMAKE_DL_LIBS} m)
endif()

# --- stb (header-only; STB_IMAGE_IMPLEMENTATION is already in material.cpp) -------
declare_dep(stb https://github.com/nothings/stb.git ${STB_REF} NO_CMAKE)
FetchContent_MakeAvailable(stb)
add_library(stb INTERFACE)
target_include_directories(stb SYSTEM INTERFACE ${stb_SOURCE_DIR})

# --- Your code ---------------------------------------------------------------------
add_subdirectory(engine)
add_subdirectory(games)
