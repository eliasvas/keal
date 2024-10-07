### Description
This is a roguelike game/engine project I've been working on. </br>
### Gameplay
TBA
### Tech
Engine side, I want things as generic as possible so that I can copy-paste the engine for other projects. </br>
To keep my sanity, currently we do OpenGL for graphics and SDL2 for everything else. </br>
Only dependency other than these is a [gui](https://github.com/eliasvas/gui) library I'm developing separately. </br>
For gameplay stuff I implemented a [bitset-based ECS](https://github.com/SanderMertens/ecs-faq?tab=readme-ov-file#bitset-based-ecs) </br>
### FAQ
- Why make a custom engine
    - because its fun, I love engine-dev.
- Will this be released commercially some day
    - probably not.
- What about assets
    - Currently placeholder art everywhere, we will think about polish later.
- How do I see the progress
    - Open items are documented [here](Todo.md)


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