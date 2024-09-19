### Description
This is a roguelike game/engine project I've been working on. </br>
You can play a somewhat recent version [here](https://eliasvas.itch.io/roguelike) </br>
Currently to be cross-platform I use OpenGL ES 3.0 and SDL2 everywhere. </br>
Main purpose of the engine is to have a lightweight framework that I can develop small games with (like this one). </br>
It has a [bitset-based ECS](https://github.com/SanderMertens/ecs-faq?tab=readme-ov-file#bitset-based-ecs) and abstractions for common game stuff like rendering/input/audio/physics/ect.. </br>
Only dependency other than SDL2 and OpenGL is a [gui](https://github.com/eliasvas/gui) library I'm developing separately. </br>

### Platform support
| Windows  | Linux | WASM |
| :-------------: | :-------------: | :-------------: |
| ✅ | ✅ | ✅ |

### Building
#### Linux
```sh
sudo apt-get install build-essential libsdl2-dev libsdl2-2.0-0 libasan6 libgles2-mesa-dev -y
git clone https://github.com/eliasvas/engine
cd engine && git submodule update --init --recursive && mkdir build
cd build && cmake .. && make --build . -j10
```
#### WASM (Emscripten)
```sh
sudo apt-get install build-essential libsdl2-dev libsdl2-2.0-0 libasan6 libgles2-mesa-dev -y
sudo apt install clang emscripten -y
git clone https://github.com/eliasvas/engine
cd engine && git submodule update --init --recursive && mkdir -p build/web
set EMCC_DEBUG=1
emcmake cmake ../..
cmake --build . -j10
emrun game.html
```