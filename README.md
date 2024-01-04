# [Phyxel](https://gitlab.com/delta_atell/phyxel) GUI
GUI implementation for the [Phyxel cellular physics library project](https://gitlab.com/delta_atell/phyxel).

As per now only the basic behavior illustrated in [Phyxel README](https://gitlab.com/delta_atell/phyxel/-/blob/main/README.md?ref_type=heads&plain=1#L1) is implemented.

The project organization is the same as in my previous project [OOP_LAB2_sem2](https://github.com/roma160/OOP_LAB2_sem2).

## Building process
 -- is taken from [OOP_LAB2_sem2 README](https://github.com/roma160/OOP_LAB2_sem2/blob/website/README.md?plain=1)

### PC build

As this project uses several external libraries ([Phyxel](https://gitlab.com/delta_atell/phyxel), [Box2D](https://github.com/erincatto/box2d), [ImGui](https://github.com/ocornut/imgui), [SDL](https://github.com/libsdl-org/SDL.git)), to build the project you will first need to init the git submodules:
```console
git submodule update --init --recursive
```

CMake is used for the project building.

Thus, building for PC would be done as usual:
```console
# Generate the CMake cache
cmake -S "." -B "./build"

# Build the sources
cmake --build "./build" --target gui
```

### [Emscripten](https://emscripten.org/) build

This is the section in which the WebAssembly executable building process is explained. Due to usage of CMake, the process is quite similar to what we have already seen in PC build.

Again, we have to pull the git submodules to get sources of our libraries:
```console
git submodule update --init --recursive
```

Next, we have to deal with the [Emscripten](https://emscripten.org/) building pipeline. General guidelines could be found and were stolen from here: 
https://stunlock.gg/posts/emscripten_with_cmake/

The next steps for the build are:
1. Install the Emscripten compiler and setup the Env variables (follow the link above for more detailed instructions);
2. Open the terminal in *ADMIN MODE* and go the source dir. Now use following commands:
```console
# Configure the build folder
emcmake cmake -S . -B ./build/emscripten_build

# Build the gui target
cmake --build ./build/emscripten_build --target gui
```
3. If there are no errors, this must be the thing. Now you could serve a Python server from an [`index_html`](./gui/index_html/) folder:
```console
python -m http.server --directory ./gui/index_html/ 8000
```
