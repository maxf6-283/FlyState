cmake_minimum_required(VERSION 3.20)

project(flystate LANGUAGES C CXX)

option(USE_JOLT "Build and link Jolt Physics from 3rdparty/JoltPhysics" OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Keep one active compilation database at build/ so clangd can use
# --compile-commands-dir=build regardless of the selected platform preset.
set(flystate_BUILD_COMPILE_COMMANDS "${CMAKE_SOURCE_DIR}/build/compile_commands.json")
if(IS_SYMLINK "${flystate_BUILD_COMPILE_COMMANDS}")
    file(REMOVE "${flystate_BUILD_COMPILE_COMMANDS}")
endif()
if(NOT EXISTS "${flystate_BUILD_COMPILE_COMMANDS}")
    file(CREATE_LINK
        "${CMAKE_BINARY_DIR}/compile_commands.json"
        "${flystate_BUILD_COMPILE_COMMANDS}"
        SYMBOLIC
    )
endif()

set(RAYLIB_SOURCE_DIR "${CMAKE_SOURCE_DIR}/3rdparty/raylib" CACHE PATH "Path to the raylib source tree")
set(JOLT_SOURCE_DIR "${CMAKE_SOURCE_DIR}/3rdparty/JoltPhysics" CACHE PATH "Path to the Jolt Physics source tree")

if(NOT EXISTS "${RAYLIB_SOURCE_DIR}/CMakeLists.txt")
    message(FATAL_ERROR
        "raylib was not found at ${RAYLIB_SOURCE_DIR}. Run ./scripts/setup-dependencies.sh first.")
endif()

if(EMSCRIPTEN)
    # raylib uses this cache variable to select its Web platform backend.
    set(PLATFORM "Web" CACHE STRING "raylib platform" FORCE)
endif()

set(BUILD_EXAMPLES OFF CACHE BOOL "Disable raylib examples" FORCE)
set(BUILD_GAMES OFF CACHE BOOL "Disable raylib games" FORCE)
add_subdirectory("${RAYLIB_SOURCE_DIR}" "${CMAKE_BINARY_DIR}/raylib")

if(USE_JOLT)
    if(NOT EXISTS "${JOLT_SOURCE_DIR}/Build/CMakeLists.txt")
        message(FATAL_ERROR
            "Jolt Physics was not found at ${JOLT_SOURCE_DIR}. Run ./scripts/setup-dependencies.sh first, or use -DUSE_JOLT=OFF.")
    endif()

    set(BUILD_SAMPLES OFF CACHE BOOL "Disable Jolt samples" FORCE)
    set(BUILD_TESTS OFF CACHE BOOL "Disable Jolt tests" FORCE)
    add_subdirectory("${JOLT_SOURCE_DIR}/Build" "${CMAKE_BINARY_DIR}/jolt")
endif()

add_executable(flystate
    "${CMAKE_SOURCE_DIR}/src/game.cpp"
    "${CMAKE_SOURCE_DIR}/src/screen_manager.cpp"
    "${CMAKE_SOURCE_DIR}/src/main_menu.cpp"
    "${CMAKE_SOURCE_DIR}/src/button.cpp"
    "${CMAKE_SOURCE_DIR}/src/level_select.cpp"
    "${CMAKE_SOURCE_DIR}/src/level.cpp"
)
target_link_libraries(flystate PRIVATE raylib)
target_compile_features(flystate PRIVATE cxx_std_20)

if(USE_JOLT)
    if(TARGET Jolt)
        target_link_libraries(flystate PRIVATE Jolt)
    elseif(TARGET Jolt::Jolt)
        target_link_libraries(flystate PRIVATE Jolt::Jolt)
    else()
        message(FATAL_ERROR "Jolt was built, but no Jolt or Jolt::Jolt CMake target was found.")
    endif()
    target_compile_definitions(flystate PRIVATE flystate_HAS_JOLT=1)
endif()

if(EMSCRIPTEN)
    file(GLOB_RECURSE GAME_ASSETS CONFIGURE_DEPENDS
        "${CMAKE_SOURCE_DIR}/assets/*"
    )
    set_property(TARGET flystate APPEND PROPERTY LINK_DEPENDS
        "${GAME_ASSETS}"
    )
    set_target_properties(flystate PROPERTIES SUFFIX ".html")
    set_property(TARGET flystate APPEND PROPERTY LINK_DEPENDS
        "${CMAKE_SOURCE_DIR}/web/shell.html"
    )
    target_link_options(flystate PRIVATE
        "-sUSE_GLFW=3"
        "-sWASM=1"
        "-sASYNCIFY"
        "-sALLOW_MEMORY_GROWTH=1"
        "--shell-file=${CMAKE_SOURCE_DIR}/web/shell.html"
        "--preload-file=${CMAKE_SOURCE_DIR}/assets@/assets"
    )
else()
    if(WIN32)
        set(flystate_PLATFORM windows)
    elseif(UNIX AND NOT APPLE)
        set(flystate_PLATFORM linux)
    else()
        set(flystate_PLATFORM desktop)
    endif()
    set_target_properties(flystate PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build/${flystate_PLATFORM}"
    )
endif()
