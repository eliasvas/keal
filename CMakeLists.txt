cmake_minimum_required(VERSION 3.19...3.29)

# set the output directory for built objects.
# This makes sure that the dynamic library goes into the build directory automatically.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
# prevent installing to system directories.
set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}" CACHE INTERNAL "")


set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED TRUE)

project(TestGame VERSION 1.0.0 LANGUAGES C)

# build engine dependencies
include(FetchContent)
Set(FETCHCONTENT_QUIET FALSE)
FetchContent_Declare( SDL2 GIT_REPOSITORY https://github.com/libsdl-org/SDL.git GIT_TAG release-2.26.3 GIT_SHALLOW TRUE GIT_PROGRESS TRUE)
FetchContent_MakeAvailable(SDL2)
if (WIN32)
    FetchContent_Declare( GLEW GIT_REPOSITORY https://github.com/Perlmint/glew-cmake.git GIT_TAG glew-cmake-2.1.0)
    FetchContent_MakeAvailable(GLEW)
endif()

# build the engine, linking SDL2 and GLEW if necessary
# TODO -- shouldn't this statically link SDL/GLEW and be a .dll
add_library(engine STATIC engine/gfx/glb.c)
include_directories(engine ${SDL2_INCLUDE_DIRS})
include_directories(engine engine)
target_link_libraries(engine SDL2::SDL2-static)
if (WIN32)
    target_link_libraries(engine libglew_static)
endif()





#build the game executable
add_executable(game game/main.c)
target_include_directories(game PUBLIC engine)
target_link_libraries(game PUBLIC engine SDL2::SDL2main)

# use Address Sanitizer (TODO -- check if this works on linux, i think it should be -fsanitize)
target_compile_options(game PUBLIC /fsanitize=address)
target_link_options(game PUBLIC /fsanitize=address)