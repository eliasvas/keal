cmake_minimum_required(VERSION 3.19...3.29)
set(ENABLE_ASAN FALSE)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED TRUE)
project(TestGame VERSION 1.0.0 LANGUAGES C)


# build the engine
add_library(engine STATIC engine/misc/gui_impl.c engine/core/entity.c engine/core/global_state.c engine/core/window.c engine/core/window_SDL_impl.c engine/core/input.c engine/gfx/ogl.c engine/gfx/rend.c engine/audio/audio.c engine/audio/audio_SDL_impl.c)
include_directories(engine ext/gui/gui/include)
include_directories(engine engine)
add_subdirectory(ext/gui)
target_link_libraries(engine guiLib)
if (DEFINED EMSCRIPTEN)
	set_target_properties(engine PROPERTIES COMPILE_FLAGS " -sWASM=1 -sSIDE_MODULE=1 -s STANDALONE_WASM -sFULL_ES3=1 -sUSE_SDL=2 -sUSE_SDL_MIXER=2 -sALLOW_MEMORY_GROWTH=1 -sMAXIMUM_MEMORY=2GB -sFORCE_FILESYSTEM=1")
	set_target_properties(engine PROPERTIES LINK_FLAGS    " -sWASM=1 -sSIDE_MODULE=1 -s STANDALONE_WASM -sFULL_ES3=1 -sUSE_SDL=2 -sUSE_SDL_MIXER=2 -sALLOW_MEMORY_GROWTH=1 -sMAXIMUM_MEMORY=2GB -sFORCE_FILESYSTEM=1 --preload-file assets")
	set(CMAKE_EXECUTABLE_SUFFIX ".wasm")
    target_link_libraries(engine GLESv2)
else()
    # regular SDL2
    option(SDL_STATIC "Build SDL2 as a static library" ON)
    option(SDL_SHARED "Build SDL2 as a shared library" OFF)
    add_subdirectory(ext/SDL)
    include_directories(engine ${SDL2_INCLUDE_DIRS})
    target_link_libraries(engine SDL2-static)
    # and SDL_Mixer
    option(SDL2MIXER_SHARED "Build SDL2_mixer as a shared library" OFF)
    option(SDL2MIXER_BUILD_SHARED_LIBS "Build SDL2_mixer shared libraries" OFF)
    option(BUILD_SHARED_LIBS "Build SDL2_mixer shared libraries" OFF)
    option(SDL2MIXER_STATIC "Build SDL2_mixer as a static library" ON)
    option(SDL2MIXER_MIDI "Support MIDI output" OFF)
    option(SDL2MIXER_MOD "Support loading MOD music" OFF)
    option(SDL2MIXER_WAVPACK "Enable WavPack music" OFF)
    add_subdirectory(ext/SDL_mixer)
    include_directories(${SDL2_MIXER_INCLUDE_DIRS})
    target_link_libraries(engine SDL2_mixer)
    # and OpenGL
    target_link_libraries(engine GL)
endif()

# use Address Sanitizer (in engine)
if(ENABLE_ASAN)
    message(STATUS "AddressSanitizer enabled")
    target_compile_options(engine PUBLIC -fsanitize=address)
    target_link_options(engine PUBLIC -fsanitize=address)
    target_compile_definitions(engine PUBLIC ENABLE_SANITIZER=1)
else()
    message(STATUS "AddressSanitizer disabled")
    target_compile_definitions(engine PUBLIC ENABLE_SANITIZER=0)
endif()

#build the game executable
add_executable(game game/main.c game/game_state.c game/actor.c game/map.c game/game_gui.c)
target_include_directories(game PUBLIC engine)
target_link_libraries(game PUBLIC engine)
if (DEFINED EMSCRIPTEN)
	set_target_properties(game PROPERTIES COMPILE_FLAGS "-sASSERTIONS=1  -sMAIN_MODULE=1 -sUSE_SDL=2 -sUSE_SDL_MIXER=2 -sMAXIMUM_MEMORY=2GB -sFORCE_FILESYSTEM=1")
	set_target_properties(game PROPERTIES LINK_FLAGS    "-sASSERTIONS=1 -sWASM=1 -sMAIN_MODULE=1 -sFULL_ES3=1 -sUSE_SDL=2 -sUSE_SDL_MIXER=2 -sALLOW_MEMORY_GROWTH=1 -sMAXIMUM_MEMORY=2GB -sFORCE_FILESYSTEM=1 --preload-file assets")
	set(CMAKE_EXECUTABLE_SUFFIX ".html")
else()
    target_link_libraries(game PUBLIC engine SDL2::SDL2main)
endif()

# move assets to BUILD_DIR/assets
set(ASSET_DIR "${CMAKE_SOURCE_DIR}/assets")
add_custom_target(copy_assets ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${ASSET_DIR} $<TARGET_FILE_DIR:game>/assets
)
add_dependencies(game copy_assets)
target_compile_definitions(game PRIVATE ASSET_DIR="assets")