![TilemapEditor](https://user-images.githubusercontent.com/63356975/169343832-a648e10d-b156-46f5-8d2a-8fd044c74144.png)
# Jade Tilemap Editor 

The Editor uses the **ECS** system from the course https://pikuma.com/courses/cpp-2d-game-engine-development. You can load and save data to [.map] files.
When saving and loading, the editor uses a [.lua] file that loads the the location of the 
[.map] files and the tilesets that are used.

This application was created to quickly make tile maps for small games created with the **Pikuma Game Engine**. I re-made the tilemap editor that I created in my 
**ZeldaClone** https://github.com/dwjclark11/ZeldaClone_NES. I added more functionality and made the code more clean and concise as my abilities continue to grow.

# Build
----
Requires [CMake 3.26](https://cmake.org/) and [vcpkg](https://github.com/microsoft/vcpkg)
#### Get VCPKG:

```ps
git clone https://github.com/microsoft/vcpkg
./vcpkg/bootstrap-vcpkg.bat
```
#### Make sure the following environment variables are set:
```ps
VCPKG_ROOT=[path_to_vcpkg]
VCPKG_DEFAULT_TRIPLET=x64-windows
```
#### Install dependencies 
```ps
./vcpkg install glm spdlog sdl2 sdl2-ttf sdl2-image lua sol2
```
  * Linux Also needs GTK-+3.0 for NFD build
```sudo apt-get install build-essential libgtk-3-dev```

#### Clone the repository 
```ps
git clone https://github.com/dwjclark11/TilemapEditor.git

cmake -S . -B build 
cd build 
cmake --build . --config Release 

cd bin 
./TilemapEditor
```

# Read the Docs
* Just created a quick documation site created with MD Book. Check it out here (Work in progress):  https://dwjclark11.github.io/TilemapEditor_Document_Site/


## Video 


https://user-images.githubusercontent.com/63356975/159281938-66793ba8-79ea-48b7-aa38-ed50d9cf23ee.mp4

## Loading a Map Project


https://user-images.githubusercontent.com/63356975/169720893-22976487-dec7-431e-8047-50c2e60b8411.mp4



## Technologies
*    **ImGui** - for user interface
*    **SDL2** - for rendering and window/inputs
*    **Sol/Lua** - for loading files
*    **ECS** - from Pikuma Game Engine 

# Dependencies
* SDL2
* SDL2_ttf
* SDL2_image
* ImGui
* SPDLOG
* Sol/Lua
* NFD 

## Functionality
* Load/Add multiple tilemaps
* Undo/Redo functionality when adding/removing tiles and changing canvas size
* Save/Load capabilities.
    *  I have changed the way that I load/save files. It differs from the course; therefore, you may have to rewrite the save functionality.
    *  The load function uses lua/sol and loads previously saved projects and their assets/tilemaps into the asset manager.
* Selecting different tiles based on where you click on the displayed tileset
* Panning and Zooming
 
## Tilemap Editor Controls

### Key Shortcuts and Functions
| **key** | **Function** | 
| --- | -------- |
| W   | Move Camara Up |
| S   | Move Camera Down |
| A   | Move Camera Left |
| D   | Move Camera Right |
| Space | Move Camera to Default Position |
| Ctrl + Z | Undo Command |
| Ctrl + Shift + Z | Redo Command | 
| Ctrl + S | Save/Save As Project |
| Ctrl + O | Open Project |
| Ctrl + N | New Canvas/Project | 

### Mouse Functions
| **Mouse Button** | **Function** |
| ------------ | -------- | 
| Left Button | Add Tile |
| Middle Button | Pan/Move the Camera |
| Right Button | Remove Tile |
| Wheel Up | Zoom In | 
| Wheel down | Zoom Out

### Issues to Work On
* Only works on 1920 1080 Resolution, does not take into affect other screen resolutions
* ~~Box Collider Offset not scaling properly with zoom~~ fixed
* There are probably more bugs that I am still finding
