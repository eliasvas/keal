### Description
This is a roguelike game/engine project I've been working on. </br>
Currently to be cross-platform I use OpenGL ES 3.0 and SDL2 everywhere. </br>
You can play a somewhat recent version [here](https://ily-gep.itch.io/wasm-demo0) </br>

### Progress Checklist
- Prerequisites
  - [x] Basic Engine Architecture
  - [x] 2D Rendering
  - [x] ECS
  - [ ] Multithreading (Emscripten Sucks)
- Playable 2D Roguelike
  - [x] make
  - [ ] real-time combat 
  - [ ] game
- Editor/GUI stuff
  - [ ] mini-map
  - [x] GUI enhancements
  - [ ] In-Game editor
- 3D stuff
    - [ ] Physically-Based Deferred Renderer
    - [ ] Skeletal Animations
    - [ ] Cascaded Shadow-Maps
    - [ ] Fog-of-War Rendering
    - [ ] Port game to 3D realm
- Improve the Game
    - [ ] TBA

### Platform support
| Windows  | Linux | WASM |
| :-------------: | :-------------: | :-------------: |
| ✅ | ✅ | ✅ |

### Building
#### Linux
```sh
sudo apt-get install build-essential libsdl2-dev libsdl2-2.0-0 libasan6 libgles2-mesa-dev -y
git clone https://github.com/eliasvas/engine
cd engine && git submodule update --init && mkdir build
cd build && cmake .. && make --build . -j10
```
#### WASM (Emscripten)
```sh
sudo apt-get install build-essential libsdl2-dev libsdl2-2.0-0 libasan6 libgles2-mesa-dev -y
sudo apt install clang emscripten -y
git clone https://github.com/eliasvas/engine
cd engine && git submodule update --init && mkdir -p build/web
set EMCC_DEBUG=1
emcmake cmake ../..
cmake --build . -j10
emrun game.html
```