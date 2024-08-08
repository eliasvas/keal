cmake_minimum_required(VERSION 3.19...3.29)
set(ENABLE_ASAN FALSE)
set(BUILD_SDL TRUE)
# we NEED -sMEMORY64 in the long run, if only for the allocators

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
add_subdirectory(ext/gui)
if (WIN32)
    add_subdirectory(ext/glew-cmake)
endif()

# build the engine
add_library(engine STATIC engine/misc/gui_impl.c engine/core/entity.c engine/core/global_state.c engine/core/window.c engine/core/window_SDL_impl.c engine/core/input.c engine/gfx/ogl.c engine/gfx/rend.c)
include_directories(engine ext/gui/gui/include)
include_directories(engine engine)
target_link_libraries(engine guiLib)


if (DEFINED EMSCRIPTEN)
	set_target_properties(engine PROPERTIES COMPILE_FLAGS " -sWASM=1 -sSIDE_MODULE=1 -s STANDALONE_WASM -sFULL_ES3=1 -sUSE_SDL=2 -sALLOW_MEMORY_GROWTH=1 -sMAXIMUM_MEMORY=2GB -sFORCE_FILESYSTEM=1")
	set_target_properties(engine PROPERTIES LINK_FLAGS    " -sWASM=1 -sSIDE_MODULE=1 -s STANDALONE_WASM -sFULL_ES3=1 -sUSE_SDL=2 -sALLOW_MEMORY_GROWTH=1 -sMAXIMUM_MEMORY=2GB -sFORCE_FILESYSTEM=1 --preload-file assets")
	set(CMAKE_EXECUTABLE_SUFFIX ".wasm")
endif()

# SDL2 stuff
if (BUILD_SDL)
    add_subdirectory(ext/SDL)
    include_directories(engine ${SDL2_INCLUDE_DIRS})
    target_link_libraries(engine SDL2-static)
endif()

# use Address Sanitizer
if(ENABLE_ASAN)
    message(STATUS "AddressSanitizer enabled")
    target_compile_options(engine PUBLIC -fsanitize=address)
    target_link_options(engine PUBLIC -fsanitize=address)
    target_compile_definitions(engine PUBLIC ENABLE_SANITIZER=1)
else()
    message(STATUS "AddressSanitizer disabled")
    target_compile_definitions(engine PUBLIC ENABLE_SANITIZER=0)
endif()

# to load function definitions for OpenGL ES 3.0
if (WIN32)
    target_link_libraries(engine libglew_static)
else()
    target_link_libraries(engine GLESv2)
endif()

#build the game executable
add_executable(game game/main.c game/game_state.c game/actor.c game/map.c)
target_include_directories(game PUBLIC engine)
target_link_libraries(game PUBLIC engine)
if (BUILD_SDL)
    target_link_libraries(game PUBLIC engine SDL2::SDL2main)
endif()
if (DEFINED EMSCRIPTEN)
	set_target_properties(game PROPERTIES COMPILE_FLAGS "-sASSERTIONS=1  -sMAIN_MODULE=1 -sUSE_SDL=2 -sMAXIMUM_MEMORY=2GB -sFORCE_FILESYSTEM=1")
	set_target_properties(game PROPERTIES LINK_FLAGS    "-sASSERTIONS=1 -sWASM=1 -sMAIN_MODULE=1 -sFULL_ES3=1 -sUSE_SDL=2 -sALLOW_MEMORY_GROWTH=1 -sMAXIMUM_MEMORY=2GB -sFORCE_FILESYSTEM=1 --preload-file assets")
	set(CMAKE_EXECUTABLE_SUFFIX ".html")
endif()



# move assets to BUILD_DIR/assets
set(ASSET_DIR "${CMAKE_SOURCE_DIR}/assets")
add_custom_target(copy_assets ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${ASSET_DIR} $<TARGET_FILE_DIR:game>/assets
)
add_dependencies(game copy_assets)

target_compile_definitions(game PRIVATE ASSET_DIR="assets")